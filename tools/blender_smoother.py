import bpy
import sys

def meshes(matcher):
    return [m for m in bpy.data.meshes if matcher(m.name) is not None]

def objs(matcher):
    return [m for m in bpy.data.objects if matcher(m.name) is not None]

#def makeSmooth(matcher):
#    for mesh in meshes(matcher):
#       for face in mesh.faces:
#            face.use_smooth = True

def makeSmooth(matcher):
    for obj in objs(matcher):
        framenum = int(obj.name[-3:])
        bpy.context.scene.frame_set(framenum)
        bpy.ops.object.select_name(name=obj.name)
        bpy.ops.object.shade_smooth()
    
def applyMat(matcher):    
    print("Applying Material")
    material = bpy.data.materials['Material']
    themeshes = objs(matcher)
    for mesh in themeshes:
        #print(mesh.name)
        #mesh.materials += [material]
        mesh.data.materials.append(material)

# Apply subdivision surfaces
# matcher(name) is used to find the appropriate objects
def applySubsurf(matcher, levels = 1, render_levels = 2):
    print("Applying subdivision")
    for obj in bpy.data.objects:        
        if obj.type=="MESH":
            res = matcher(obj.name)
            if res is None:
                continue
            
            # else res is the frame number
            frame_no = res
   
            print("%d "%frame_no,end=" ")
            sys.stdout.flush()
            
            bpy.context.scene.frame_set(frame_no)            
            bpy.context.scene.objects.active = obj
        
            # add modifier
            try:
                bpy.ops.object.modifier_add(type='SUBSURF')
                mod = obj.modifiers[-1]
                mod.levels = levels
                mod.render_levels = render_levels
            except TypeError:
                pass

def matcher(name):
    if (not name.startswith('Mesh.')):
        return None
    else:
        dot_index = name.find(".")                
        return int(name[1+dot_index:])

applyMat(matcher)
#makeSmooth(matcher)
#applySubsurf(matcher,0,1)