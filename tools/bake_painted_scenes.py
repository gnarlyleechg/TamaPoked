#!/usr/bin/env python3
"""Bake original generated 6x5 artwork atlas into optional read-only TPA1 plates."""
import argparse
import hashlib
import json
import struct
import zlib
from pathlib import Path
from PIL import Image, ImageEnhance, ImageOps
import numpy as np

THEMES = ('Cave Pool Power Garden Dojo Observatory Volcano Desert Glacier Temple Arena Aviary Forest Factory Reef Theater Mine Swamp Harbor Studio Library Castle Summit Beach Circuit Gallery').split()
LOCATIONS = [
 'Cave Pool Power Garden Dojo Observatory Volcano Desert Glacier Dojo Temple Castle Arena',
 'Aviary Forest Studio Temple Dojo Factory Glacier Cave Observatory Garden Dojo Theater Castle',
 'Gallery Beach Power Volcano Arena Aviary Observatory Reef Theater Temple Glacier Harbor Pool',
 'Mine Garden Theater Dojo Swamp Factory Glacier Circuit Forest Desert Volcano Library Castle',
 'Studio Theater Gallery Circuit Mine Harbor Castle Reef Library Theater Observatory Dojo Summit',
 'Forest Cave Arena Garden Power Gallery Observatory Glacier Volcano Pool Castle Temple Theater',
 'Beach Reef Volcano Forest Observatory Studio Harbor Desert Dojo Cave Library Aviary Summit',
]
REGIONS = 'Kanto Johto Hoenn Sinnoh Unova Kalos Alola'.split()
TRAINING = ['Boxing gym', 'Reaction laboratory', 'Ball court']

def bake(atlas, dest):
    source = Image.open(atlas).convert('RGB')
    if source.width % 6 or source.height % 5:
        raise ValueError('Expected equal-sized 6x5 atlas cells')
    cw, ch = source.width // 6, source.height // 5
    if min(cw, ch) < 180:
        raise ValueError('Atlas cells too small')
    dest = Path(dest); scenes = dest / 'scenes'; scenes.mkdir(parents=True, exist_ok=True)
    records = []
    for key in range(94):
        if key < 3:
            tile, label, height = key, TRAINING[key], 233
        else:
            region, trainer = divmod(key - 3, 13)
            theme = LOCATIONS[region].split()[trainer]
            tile, label, height = 3 + THEMES.index(theme), f'{REGIONS[region]} {trainer+1}: {theme}', 127
        x, y = tile % 6 * cw, tile // 6 * ch
        # The generated atlas has fine tile boundaries; omit them from each crop.
        art = source.crop((x+3, y+3, x+cw-3, y+ch-3))
        if key >= 3:
            # Original theme plates reused with location-specific framing and grade.
            # A small horizontal pan and zoom make repeated themes visibly distinct.
            zoom = (key * 7) % 9
            left = (key * 13) % 11
            art = art.crop((left, zoom, art.width - (10-left), art.height - (8-zoom)))
            art = ImageOps.fit(art, (233, height), Image.Resampling.LANCZOS,
                               centering=(0.5, 0.46 + ((key*3)%7-3)*0.035))
            art = ImageEnhance.Color(art).enhance(0.91 + (key%9)*0.025)
            art = ImageEnhance.Brightness(art).enhance(0.93 + ((key*5)%11)*0.014)
        else:
            art = ImageOps.fit(art, (233, height), Image.Resampling.LANCZOS)
        a = np.asarray(art, dtype=np.uint16)
        pixels = (((a[:,:,0]>>3)<<11)|((a[:,:,1]>>2)<<5)|(a[:,:,2]>>3)).astype('<u2').tobytes()
        header = struct.pack('<4sHHHHII', b'TPA1', key, 233, height, 0, len(pixels), zlib.crc32(pixels))
        data = header + pixels; name = f'p{key:03}.rgb'
        (scenes/name).write_bytes(data)
        records.append(dict(id=key, label=label, tile=tile, width=233, height=height, bytes=len(data), sha256=hashlib.sha256(data).hexdigest()))
    manifest = dict(format='TPA1', atlas_sha256=hashlib.sha256(Path(atlas).read_bytes()).hexdigest(), scenes=records)
    (dest/'artwork-manifest.json').write_text(json.dumps(manifest, indent=2)+'\n')
    return manifest

if __name__ == '__main__':
    p=argparse.ArgumentParser(); p.add_argument('atlas', type=Path); p.add_argument('destination', type=Path); a=p.parse_args()
    m=bake(a.atlas,a.destination); print(len(m['scenes']), 'plates;', sum(s['bytes'] for s in m['scenes']), 'bytes')
