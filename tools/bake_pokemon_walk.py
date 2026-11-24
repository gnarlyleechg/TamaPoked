"""Bake authored textured limb layers into walk frames, using rigid rotations.
Input layers share a canvas and are supplied by the artist, not inferred
from a frontal portrait. Each limb has root, joint, foot and two PNG layers.
"""
import math
from PIL import Image
from pokemon_motion import solve_joint,paw_target,GAITS

def rotate(layer,a,b,c,d):
    angle=math.atan2(d[1]-c[1],d[0]-c[0])-math.atan2(b[1]-a[1],b[0]-a[0])
    co,si=math.cos(angle),math.sin(angle)
    return layer.transform(layer.size,Image.Transform.AFFINE,
        (co,si,a[0]-co*c[0]-si*c[1],-si,co,a[1]+si*c[0]-co*c[1]),
        resample=Image.Resampling.BICUBIC,fillcolor=(0,0,0,0))

def bake(dex,body,limbs,count=12):
    if len(limbs)!=len(GAITS[dex][0]) or not 2<=count<=24:
        raise ValueError("incorrect limb count or frame count")
    if max(body.size)>192: raise ValueError("canvas exceeds 192 pixels")
    frames=[]
    for i in range(count):
        phase=i/count
        out=Image.new("RGBA",body.size)
        layers=[]
        for j,(a,b,c,upper,lower) in enumerate(limbs):
            if upper.size!=body.size or lower.size!=body.size:
                raise ValueError("layer canvas mismatch")
            dx,dy,_=paw_target(dex,j,phase,math.dist(a,b)+math.dist(b,c))
            knee,end=solve_joint(a,b,c,a,(c[0]+dx,c[1]+dy))
            leg=rotate(lower,b,c,knee,end)
            leg.alpha_composite(rotate(upper,a,b,a,knee))
            layers.append(leg)
        for j in range(0,len(layers),2):out.alpha_composite(layers[j])
        out.alpha_composite(body)
        for j in range(1,len(layers),2):out.alpha_composite(layers[j])
        frames.append(out)
    return frames

def pack_walk(frames, idle=None):
    """TPK2 pilot with idle and both walk directions. Shared 255-color palette."""
    import struct
    w,h=frames[0].size
    if not 1<=w<=192 or not 1<=h<=192 or not 2<=len(frames)<=24:
        raise ValueError("invalid dimensions")
    idle=frames[:1] if idle is None else idle
    if not 1<=len(idle)<=24: raise ValueError("invalid idle count")
    allframes=frames+idle
    atlas=Image.new("RGB",(w*len(allframes),h))
    for i,fr in enumerate(allframes):
        if fr.size!=(w,h):raise ValueError("frame size mismatch")
        atlas.paste(fr.convert("RGB"),(i*w,0))
    palette=atlas.quantize(colors=255)
    colors=palette.getpalette()[:765]
    colors += [0]*(765-len(colors))
    out=bytearray(b"TPK2"+struct.pack("<BH",3,255))
    for r,g,b in zip(colors[::3],colors[1::3],colors[2::3]):
        out.extend(struct.pack("<H",((r>>3)<<11)|((g>>2)<<5)|(b>>3)))
    for aid,seq in [(0,idle),(1,frames),(2,[fr.transpose(Image.Transpose.FLIP_LEFT_RIGHT) for fr in frames])]:
        out.extend(struct.pack("<4B",aid,w,h,len(seq)))
        out.extend(struct.pack("<"+"H"*len(seq),*([80]*len(seq))))
        for fr in seq:
            indices=fr.convert("RGB").quantize(palette=palette,dither=Image.Dither.NONE)
            out.extend(255 if a<128 else p for p,a in zip(indices.getdata(),fr.getchannel("A").getdata()))
    return bytes(out)

if __name__=="__main__":
    import argparse,json
    from pathlib import Path
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("rig",type=Path)
    p.add_argument("--out",type=Path,required=True)
    a=p.parse_args()
    spec=json.loads(a.rig.read_text())
    def layer(name):
        with Image.open(a.rig.parent/name) as im:return im.convert("RGBA")
    frames=bake(spec["dex"],layer(spec["body"]),[
        (l["root"],l["joint"],l["foot"],layer(l["upper"]),layer(l["lower"]))
        for l in spec["limbs"]])
    a.out.mkdir(parents=True,exist_ok=True)
    # Pilot only: never overwrite the full original action pack.
    (a.out/"walk-pilot.tpk2").write_bytes(pack_walk(frames))
    frames[0].save(a.out/"walk-pilot.gif",save_all=True,append_images=frames[1:],duration=80,loop=0,disposal=2)
