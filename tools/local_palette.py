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
