"""Run with blender -b -t 2 --python-exit-code 1 --python tools/render_glb_walk.py -- model.glb output_dir clip.
Only use locally authorized model files. No network or publication.
"""
import bpy,sys,math
from pathlib import Path
from mathutils import Vector
model,out,clip=sys.argv[sys.argv.index("--")+1:]
out=Path(out).resolve();out.mkdir(parents=True,exist_ok=True)
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.gltf(filepath=str(Path(model).resolve()))
scene=bpy.context.scene
actions=list(bpy.data.actions)
print("Imported clips:",[a.name for a in actions])
matches=[a for a in actions if clip.lower() in a.name.lower()]
if len(matches)!=1:raise RuntimeError("Select exactly one original animation")
action=matches[0]
rigs=[o for o in scene.objects if o.type=="ARMATURE"]
if len(rigs)!=1:raise RuntimeError("Expected one skeletal rig")
rig=rigs[0];ad=rig.animation_data_create()
for track in ad.nla_tracks:track.mute=True
ad.action=action
start,end=action.frame_range
if end<=start:raise RuntimeError("Empty animation")
def bounds():
    dg=bpy.context.evaluated_depsgraph_get()
    points=[]
    for obj in scene.objects:
        if obj.type=="MESH":
            ev=obj.evaluated_get(dg)
            points.extend(ev.matrix_world@Vector(p) for p in ev.bound_box)
    return points
frames=[start+(end-start)*i/12 for i in range(12)]
points=[]
for f in frames:
    scene.frame_set(int(f),subframe=f-int(f));points.extend(bounds())
lo=Vector(tuple(min(p[i] for p in points) for i in range(3)))
hi=Vector(tuple(max(p[i] for p in points) for i in range(3)))
center=(lo+hi)/2;size=max(hi-lo)
if size<=0:raise RuntimeError("Empty mesh")
def aim(obj,target):obj.rotation_euler=(target-obj.location).to_track_quat("-Z","Y").to_euler()
bpy.ops.object.camera_add(location=center+Vector((-2,-3,1.3))*size)
cam=bpy.context.object;aim(cam,center);cam.data.type="ORTHO";cam.data.ortho_scale=size*1.5;scene.camera=cam
for loc,power in [((-3,-4,5),650),((3,-2,2),300),((0,3,4),450)]:
    bpy.ops.object.light_add(type="AREA",location=center+Vector(loc)*size)
    light=bpy.context.object;light.data.energy=power*size**2;light.data.shape="DISK";light.data.size=size*3;aim(light,center)
scene.world=bpy.data.worlds.new("Ambient");scene.world.use_nodes=True
scene.world.node_tree.nodes["Background"].inputs[0].default_value=(.3,.3,.3,1)
scene.render.engine="CYCLES";scene.cycles.samples=16
scene.render.resolution_x=192;scene.render.resolution_y=192;scene.render.resolution_percentage=100
scene.render.film_transparent=True;scene.render.image_settings.file_format="PNG";scene.render.image_settings.color_mode="RGBA"
for i,f in enumerate(frames):
    scene.frame_set(int(f),subframe=f-int(f))
    scene.render.filepath=str(out/f"walk-{i:02d}.png")
    bpy.ops.render.render(write_still=True)
print("Rendered",len(frames),"original skeletal frames")
