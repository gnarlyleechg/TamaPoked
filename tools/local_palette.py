from collections import Counter
from PIL import Image

def rgb(c):
    return ((c>>11)*255//31,((c>>5)&63)*255//63,(c&31)*255//31)

def allocate(old,new,old_data,new_data):
    # Reserve colors actually used by retained actions, not unused entries.
    used=sorted({x for data in old_data for x in data if x!=255})
    pal=list(dict.fromkeys(old[x] for x in used))
    freq=Counter(x for data in new_data for x in data if x!=255)
    for x,_ in freq.most_common():
        if new[x] not in pal and len(pal)<255: pal.append(new[x])
    if not pal: raise ValueError('empty palette')
    def mapping(src):
        return [min(range(len(pal)),key=lambda i:sum((a-b)**2 for a,b in zip(rgb(c),rgb(pal[i])))) for c in src]
    return pal,mapping(old),mapping(new)

def merge_local(original,pilot):
    import struct
    from merge_walk_pilot import read
    old,acts=read(original); new,walk=read(pilot)
    retained={k:v for k,v in acts.items() if k not in (1,2)}
    pal,om,nm=allocate(old,new,[v[4] for v in retained.values()],[walk[k][4] for k in (1,2)])
    out=bytearray(b"TPK2"+struct.pack("<BH",len(retained)+2,len(pal)))
    out.extend(struct.pack("<"+"H"*len(pal),*pal))
    for k,v in sorted({**retained,1:walk[1],2:walk[2]}.items()):
        w,h,n,ms,data=v
        m=nm if k in (1,2) else om
        out.extend(struct.pack("<4B",k,w,h,n)+ms+bytes(255 if x==255 else m[x] for x in data))
    return bytes(out)

def equivalent(oldpal,oldact,pal,act):
    return oldact[:4]==act[:4] and len(oldact[4])==len(act[4]) and all(
        (a==b==255) or (a!=255 and b!=255 and oldpal[a]==pal[b])
        for a,b in zip(oldact[4],act[4]))
