"""Original shaded Bulbasaur side-view pilot, not official artwork.
Generate separate overlapping limb textures and bake the real walk format.
"""
from PIL import Image,ImageDraw
from bake_pokemon_walk import bake,pack_walk
from pathlib import Path
import sys
S=160
def canvas():return Image.new("RGBA",(S,S))
def oval(im,box,color):
    d=ImageDraw.Draw(im);d.ellipse(box,fill=color,outline=(37,75,64),width=2)
    x,y,X,Y=box
    d.ellipse((x+5,y+4,X-7,Y-9),fill=tuple(min(255,c+15) for c in color))
def segment(a,b,r,color):
    im=canvas();d=ImageDraw.Draw(im)
    d.line((a,b),fill=(37,75,64),width=2*r+2)
    d.line((a,b),fill=color,width=2*r)
    for x,y in (a,b):d.ellipse((x-r,y-r,x+r,y+r),fill=color)
    d.line(((a[0]-2,a[1]),(b[0]-2,b[1])),fill=tuple(min(255,c+20) for c in color),width=3)
    return im
body=canvas()
oval(body,(39,65,137,119),(90,166,141))
d=ImageDraw.Draw(body)
d.polygon([(66,77),(61,48),(77,22),(94,32),(112,18),(131,46),(129,73),(104,86)],fill=(55,115,57),outline=(30,70,38),width=2)
d.polygon([(77,22),(86,48),(79,72),(94,82),(99,48),(112,18),(119,48),(112,74)],fill=(83,151,67))
oval(body,(14,63,83,111),(105,181,153))
d=ImageDraw.Draw(body)
d.polygon([(21,74),(22,49),(42,68)],fill=(105,181,153),outline=(37,75,64),width=2)
d.polygon([(58,66),(73,48),(77,77)],fill=(105,181,153),outline=(37,75,64),width=2)
d.polygon([(32,67),(45,72),(39,82),(28,77)],fill=(44,116,99))
d.polygon([(93,91),(108,83),(116,97),(103,105)],fill=(44,116,99))
d.ellipse((23,82,43,99),fill=(245,238,222),outline=(37,75,64))
d.ellipse((24,83,34,98),fill=(169,45,57));d.ellipse((25,85,29,90),fill="white")
d.arc((15,93,49,106),10,150,fill=(37,75,64),width=2)
limbs=[]
for i,(a,b,c) in enumerate([((53,103),(48,119),(52,139)),((66,105),(63,122),(59,139)),((112,103),(120,120),(116,139)),((126,103),(132,121),(129,139))]):
    color=(75,140,121) if i%2==0 else (99,175,147)
    limbs.append((a,b,c,segment(a,b,10,color),segment(b,c,9,color)))
frames=bake(1,body,limbs)
out=Path(sys.argv[1]);out.mkdir(parents=True,exist_ok=True)
(out/"bulbasaur-pilot.tpk2").write_bytes(pack_walk(frames))
frames[0].save(out/"bulbasaur-walk.gif",save_all=True,append_images=frames[1:],duration=80,loop=0,disposal=2)
body.save(out/"body.png")
for i,(_,_,_,u,l) in enumerate(limbs):u.save(out/f"upper-{i}.png");l.save(out/f"lower-{i}.png")
assert len({f.tobytes() for f in frames})==12
print("PASS: 12 distinct authored Bulbasaur walk frames")
