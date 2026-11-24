"""Import a locally permitted GLB without modifying tracked game assets."""
import argparse, hashlib, json, subprocess, tempfile, shutil
from pathlib import Path
from PIL import Image
from bake_pokemon_walk import pack_walk
from merge_walk_pilot import read
from local_palette import merge_local, equivalent

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("model",type=Path)
    p.add_argument("--dex",type=int,required=True)
    p.add_argument("--clip",required=True)
    p.add_argument("--idle-clip",help="Matching native idle animation")
    p.add_argument("--original",type=Path,required=True)
    p.add_argument("--out",type=Path,required=True)
    p.add_argument("--rights",required=True,help="Record permission or permitted local use basis")
    p.add_argument("--blender",default="blender")
    a=p.parse_args()
    repo=Path(__file__).resolve().parents[1]
    out=a.out.resolve()
    if out==repo or repo in out.parents:
        p.error("Output must be outside the repository")
    if not 1<=a.dex<=1025 or not a.rights.strip():
        p.error("Invalid species or rights statement")
    if out.exists():
        p.error("Choose a new output directory")
    name=f"p{a.dex:03d}.bin"
    if a.original.name!=name:
        p.error("Original sprite filename must match species")
    model=a.model.resolve()
    original=a.original.resolve()
    oldpal,oldacts=read(original)
    with tempfile.TemporaryDirectory() as tmp:
        root=Path(tmp)
        subprocess.run([a.blender,"-b","-t","2","--python-exit-code","1",
            "--python",str(repo/"tools/render_glb_walk.py"),"--",
            str(model),str(root),a.clip],check=True)
        frames=[Image.open(root/f"walk-{i:02d}.png").convert("RGBA") for i in range(12)]
        if any(f.size!=(192,192) or not f.getchannel("A").getbbox() for f in frames):
            raise ValueError("Invalid rendered frames")
        if len({f.tobytes() for f in frames})<2:
            raise ValueError("Animation is static")
        idle=None
        if a.idle_clip:
            idle_dir=root/"idle"
            subprocess.run([a.blender,"-b","-t","2","--python-exit-code","1",
                "--python",str(repo/"tools/render_glb_walk.py"),"--",
                str(model),str(idle_dir),a.idle_clip],check=True)
            idle=[Image.open(idle_dir/f"walk-{i:02d}.png").convert("RGBA") for i in range(12)]
            if any(f.size!=(192,192) or not f.getchannel("A").getbbox() for f in idle):
                raise ValueError("Invalid idle frames")
        replace=(0,1,2) if idle else (1,2)
        pilot=root/"pilot.tpk2"
        pilot.write_bytes(pack_walk(frames,idle))
        staged=root/name
        staged.write_bytes(merge_local(original,pilot,replace))
        pal,acts=read(staged)
        if any(not equivalent(oldpal,v,pal,acts[k]) for k,v in oldacts.items() if k not in replace):
            raise ValueError("Original actions changed")
        out.mkdir(parents=True)
        shutil.copyfile(staged,out/name)
        frames[0].save(out/"walk.gif",save_all=True,append_images=frames[1:],duration=80,loop=0,disposal=2)
        (out/"provenance.json").write_text(json.dumps({"dex":a.dex,"clip":a.clip,"idle_clip":a.idle_clip,
            "source":model.name,"sha256":hashlib.sha256(model.read_bytes()).hexdigest(),
            "rights":a.rights,"distribution":"local only","sprite":name},indent=2))
    print("Local sprite ready:",out/name)
    print("Original non-replaced actions preserved. Review walk.gif before installing.")
if __name__=="__main__":
    main()
