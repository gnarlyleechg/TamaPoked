"""Merge pilot walk actions into a temporary original TPK2 pack.
Preserve every other action, remapping palettes into a shared 255-color palette.
"""
import struct
from pathlib import Path
from PIL import Image
import sys

def read(path):
    b=Path(path).read_bytes()
    if b[:4]!=b"TPK2":raise ValueError("magic")
    n,p=struct.unpack_from("<BH",b,4)
    pal=struct.unpack_from("<"+"H"*p,b,7)
    pos=7+2*p; acts={}
    for _ in range(n):
        aid,w,h,nf=struct.unpack_from("<4B",b,pos);pos+=4
        size=2*nf+w*h*nf
        if aid in acts or not w or not h or not nf or pos+size>len(b):raise ValueError("action")
        acts[aid]=(w,h,nf,b[pos:pos+2*nf],b[pos+2*nf:pos+size])
        pos+=size
    if pos!=len(b):raise ValueError("trailing bytes")
    return pal,acts

def merge(original,pilot):
    old,acts=read(original);new,walk=read(pilot)
    # Preserve original palette and all non-walk action bytes exactly.
    pal=list(old[:255]); lookup={c:i for i,c in enumerate(pal)}
    for c in new:
        if c not in lookup and len(pal)<255:
            lookup[c]=len(pal);pal.append(c)
    def rgb(c):return ((c>>11)*255//31,((c>>5)&63)*255//63,(c&31)*255//31)
    mapping=[]
    for c in new:
        if c in lookup:mapping.append(lookup[c]);continue
        r=rgb(c);mapping.append(min(range(len(pal)),key=lambda i:sum((a-b)**2 for a,b in zip(r,rgb(pal[i])))))
    for aid in (1,2):
        w,h,nf,ms,data=walk[aid]
        acts[aid]=(w,h,nf,ms,bytes(255 if x==255 else mapping[x] for x in data))
    out=bytearray(b"TPK2"+struct.pack("<BH",len(acts),len(pal)))
    out.extend(struct.pack("<"+"H"*len(pal),*pal))
    for aid,(w,h,nf,ms,data) in sorted(acts.items()):
        out.extend(struct.pack("<4B",aid,w,h,nf)+ms+data)
    return bytes(out)
if __name__=="__main__":
    original,pilot,out=sys.argv[1:]
    Path(out).write_bytes(merge(original,pilot))
    p,a=read(original);q,b=read(out)
    assert q[:len(p)]==p
    assert all(b[k]==v for k,v in a.items() if k not in (1,2))
    assert b[1][2]==b[2][2]==12
    print("PASS: both walk actions replaced; all other original action bytes preserved")
