import bpy,sys
from mathutils import Vector
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.mesh.primitive_uv_sphere_add(segments=24,ring_count=16)
mesh=bpy.context.object
mat=bpy.data.materials.new("Fixture teal");mat.use_nodes=True
mat.node_tree.nodes.get("Principled BSDF").inputs["Base Color"].default_value=(.08,.45,.32,1)
mesh.data.materials.append(mat)
bpy.ops.object.armature_add()
rig=bpy.context.object
mesh.parent=rig
mod=mesh.modifiers.new("Skin","ARMATURE");mod.object=rig
group=mesh.vertex_groups.new(name=rig.data.bones[0].name)
group.add(list(range(len(mesh.data.vertices))),1,"REPLACE")
bone=rig.pose.bones[0];bone.rotation_mode="XYZ"
for f,angle in [(1,-.25),(7,.25),(13,-.25)]:
    bone.rotation_euler.y=angle
    bone.keyframe_insert(data_path="rotation_euler",frame=f)
rig.animation_data.action.name="fixture_walk"
bpy.context.scene.frame_end=13
bpy.ops.export_scene.gltf(filepath=sys.argv[-1],export_format="GLB")
