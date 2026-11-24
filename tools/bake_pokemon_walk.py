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
