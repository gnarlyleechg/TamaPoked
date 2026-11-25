"""Offline two-bone motion core for authored Pokemon limb layers.
Coordinates are in source-art pixels. No runtime ESP32 computation.
"""
import math

def solve_joint(a,b,c,root,target):
    l1,l2=math.dist(a,b),math.dist(b,c)
    if min(l1,l2)<=0: raise ValueError("zero length bone")
    dx,dy=target[0]-root[0],target[1]-root[1]
    raw=math.hypot(dx,dy)
    if raw<1e-9:
        dx,dy=c[0]-a[0],c[1]-a[1]
        raw=math.hypot(dx,dy)
        if raw<1e-9: dx,dy,raw=1,0,1
    ux,uy=dx/raw,dy/raw
    d=max(abs(l1-l2)+1e-6,min(l1+l2-1e-6,raw))
    p=(l1*l1+d*d-l2*l2)/(2*d)
    off=math.sqrt(max(0,l1*l1-p*p))
    sign=1 if (c[0]-a[0])*(b[1]-a[1])-(c[1]-a[1])*(b[0]-a[0])>=0 else -1
    return (root[0]+ux*p-uy*off*sign,root[1]+uy*p+ux*off*sign),(root[0]+ux*d,root[1]+uy*d)

# Phase offsets, stance fraction, normalized reach and lift.
# These are initial motion targets, not validated species artwork landmarks.
GAITS={
    1: ((.25,.75,.50,0),.65,.12,.10),
    4: ((0,.5),.62,.15,.12),
    7: ((0,.5),.68,.10,.08),
    25: ((0,.5),.60,.13,.14),
}

def paw_target(dex,limb,phase,length):
    offsets,stance,reach,lift=GAITS[dex]
    t=(phase+offsets[limb])%1
    reach*=length
    if t<stance: return (-reach+2*reach*t/stance,0,True)
    q=(t-stance)/(1-stance)
    return (reach-2*reach*q*q*(3-2*q),-lift*length*math.sin(math.pi*q),False)
