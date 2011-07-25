# bens script for animating a bunch of objs

import bpy

# iterator that returns a (frame_number, object) pair 
def allFrames(numlength,prefix):
    for obj in bpy.data.objects:
        if obj.type=="MESH" and obj.name.startswith(prefix):
            try: 
                yield (int(obj.name[-numlength:]),obj)
            except Exception as e:
                pass

def animate(frame_num,obj):
    # Basically Bart's code from io.py:_animate_object
    for frame, hidden in zip((1, frame_num, frame_num + 1),(True, False, True)):
        bpy.context.scene.frame_set(frame)
        obj.hide = hidden
        obj.hide_render = hidden
        obj.keyframe_insert("hide")
        obj.keyframe_insert("hide_render")

BASE_DIR = "C:\\dev\\playground\\luavcg\\build-win-eclipse"
N = 721

# import
for i in range(N):
    print(str(i) + "/" + str(N))
    filename = "out%04d.obj"%i
    bpy.ops.import_scene.obj(filepath=BASE_DIR+"\\"+filename)

print("Don't forget to delete first frame!")

# animate        
for (f,o) in allFrames(3,"Mesh"):
    animate(f,o)
