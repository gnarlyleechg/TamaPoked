#!/usr/bin/env python3
"""Stage an app-only candidate. Never connects to or flashes a device."""
import argparse
import hashlib
import json
import re
import struct
from pathlib import Path
import zipfile

FLASH_SIZE = 16 * 1024 * 1024
APP_OFFSET = 0x10000
TABLE_MAX = 0x1000


def partitions(data):
    if not data or len(data) > TABLE_MAX or len(data) % 32:
        raise ValueError('Invalid partition table length')
    entries = []
    labels = set()
    ended = False
    for pos in range(0, len(data), 32):
        raw = data[pos:pos+32]
        magic, typ, sub, offset, size, label, flags = struct.unpack('<HBBII16sI', raw)
        if magic == 0xffff:
            if raw != b'\xff'*32 or any(b != 255 for b in data[pos:]):
                raise ValueError('Invalid partition padding')
            ended = True
            break
        if magic == 0xebeb:
            if raw[:16] != b'\xeb\xeb'+b'\xff'*14 or hashlib.md5(data[:pos]).digest() != raw[16:]:
                raise ValueError('Partition MD5 mismatch')
            if any(b != 255 for b in data[pos+32:]):
                raise ValueError('Unexpected entries after partition MD5')
            ended = True
            break
        if magic != 0x50aa:
            raise ValueError('Invalid partition entry magic')
        name = label.split(b'\0', 1)[0].decode('ascii')
        if not name or name in labels or not size or offset < 0x9000 or offset % 4096 or size % 4096 or offset+size > FLASH_SIZE:
            raise ValueError('Invalid partition bounds or duplicate label')
        if typ == 0 and offset % 65536:
            raise ValueError('Unaligned application')
        labels.add(name)
        entries.append(dict(name=name,type=typ,subtype=sub,offset=offset,size=size,flags=flags))
    if not ended or not entries:
        raise ValueError('Missing partition table terminator')
    ordered=sorted(entries,key=lambda e:e['offset'])
    if any(a['offset']+a['size'] > b['offset'] for a,b in zip(ordered,ordered[1:])):
        raise ValueError('Overlapping partitions')
    return entries


def plan(app, candidate_table, baseline_table):
    # Compare parsed entries including flags, names and sizes. Padding may differ.
    candidate=partitions(candidate_table)
    baseline=partitions(baseline_table)
    if candidate != baseline:
        raise ValueError('Partition layout differs from installed baseline; stop')
    slots=[e for e in candidate if e['type']==0 and e['offset']==APP_OFFSET and e['subtype'] in (0,0x10)]
    if len(slots)!=1:
        raise ValueError('No unique supported app at 0x10000')
    # This is a structural gate, not a replacement for esptool image-info.
    if len(app)<24 or app[0]!=0xe9 or not 1<=app[1]<=16 or struct.unpack_from('<H',app,12)[0]!=9:
        raise ValueError('Not a recognizable ESP32-S3 application image')
    slot=slots[0]
    if len(app)>slot['size'] or ((len(app)+4095)//4096)*4096>slot['size']:
        raise ValueError('Application or sector erase extent exceeds slot')
    if not any(e['type']==1 and e['subtype']==2 for e in candidate):
        raise ValueError('No NVS partition in baseline')
    return dict(app_offset=APP_OFFSET,app_bytes=len(app),app_sha256=hashlib.sha256(app).hexdigest(),
                expected_layout=baseline,erase_all=False,hardware_gate='Verify installed layout and active boot slot; backup saves; owner approval required')


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--app',type=Path,required=True); p.add_argument('--partitions',type=Path,required=True)
    p.add_argument('--installed-baseline',type=Path,required=True,help='Trusted partition table from installed build/device, not candidate')
    p.add_argument('--commit',required=True); p.add_argument('--out',type=Path,required=True)
    a=p.parse_args()
    if not re.fullmatch('[0-9a-f]{40}',a.commit):p.error('Full source commit SHA required')
    if a.partitions.resolve()==a.installed_baseline.resolve():p.error('Baseline must be an independent file')
    if a.out.exists():p.error('Output exists; refusing overwrite')
    app=a.app.read_bytes(); baseline=a.installed_baseline.read_bytes(); candidate=a.partitions.read_bytes()
    info=plan(app,candidate,baseline);info['source_commit']=a.commit
    # Never put a partition table, bootloader, or boot_app0 in the flash payload.
    readme='''PRIVATE APP-ONLY CANDIDATE — NOT AUTHORIZED TO FLASH
Before flashing: verify the actual installed device's partition table against
expected_layout in release.json; confirm the active boot slot is 0x10000 and
flash encryption/secure boot compatibility; back up existing saves and SD.
Run esptool image-info on app.bin. Do not erase flash or format the SD card.
Only after all gates pass and owner approves, flash ONLY app.bin at 0x10000
using the correct port and standard esptool write-flash command without erase-all.
This package does not itself verify the connected board or perform flashing.
'''
    with zipfile.ZipFile(a.out,'x',zipfile.ZIP_DEFLATED) as z:
        z.writestr('app.bin',app);z.writestr('release.json',json.dumps(info,indent=2)+'\n');z.writestr('README.txt',readme)
    print('Staged app-only candidate:',a.out)

if __name__=='__main__':main()
