########################################
# Bens script for:
# - importing a bunch of objs as an animation
# - applying a material, smoothing, and subdiv
#   to the imported objects
#
# The script works by importing a bunch of files that 
# match the regex: ".*[0-9]+\.obj" 
# (e.g., test_0000.obj, test94.obj, some_fileX00334.obj)
# The number suffix indicates the frame in which the object
# will appear. Multiple objects can appear in the same frame.
#
# Usage: set the USER-DEFINED vars then run the script.
#
# Warning: This script requires that there is
# no object named "Mesh" in the scene. 
#
########################################

IMPORT, APPLY_MATERIAL, APPLY_SMOOTH, APPLY_SUBDIV, APPLY_ALL = range(5)

########## USER-DEFINED VARS ###########

# Set Mode to one of the above and run the script
# Typically, you'll want to run:
# IMPORT then APPLY_MATERIAL then APPLY_SMOOTH then APPLY_SUBDIV
# or requivalently IMPORT then APPLY_ALL
Mode = IMPORT

# ObjGlob: The glob that matches files to import
ObjGlob = "C:\\dev\\fg\\bin\\james\\*.obj"

# ApplyNameRegex: All objects in Blender matching 
# this will get applied to (in python regex form)
# must end in numbers (e.g., have [0-9]+ at the 
# end of the regex)
ApplyNameRegex = "james.*[0-9]+"

# Material Vars
MaterialName = "Material"

# Subdiv Vars
ViewSubdiv = 1
RenderSubdiv = 2

########## script starts here ##########

import bpy, glob, sys, os.path, re

numberOfFrames = 1 # records the number of frames imported

def animate(frame_num,obj):
    # Basically Bart Veldstra's code from io.py:_animate_object
    for frame, hidden in zip((0, frame_num, frame_num + 1),(True, False, True)):
        bpy.context.scene.frame_set(frame)
        obj.hide = hidden
        obj.hide_render = hidden
        obj.keyframe_insert("hide")
        obj.keyframe_insert("hide_render")

def frameNumFromName(name):
    match = re.search("([0-9]+)$",name)
    if (match is None):
        raise(RuntimeError("Couldnt find frame number in %s"%name))
        
    return int(match.group(1))
    
def importAndAnimateObj(fp):
    global numberOfFrames
    
    filename = os.path.split(fp)[1]
    # import and rename obj
    bpy.ops.import_scene.obj(filepath=fp)
    obj = bpy.data.objects["Mesh"]
    obj.name = filename[:-4]
    # extract frame number
    frameno = frameNumFromName(obj.name)
    # finally animate
    animate(frameno,obj)
    # and record the max frame
    numberOfFrames = max(numberOfFrames,frameno)

def importAndAnimate():
    global numberOfFrames
    
    # first check that Mesh doesnt exist
    if "Mesh" in bpy.data.objects:
        raise(RuntimeError("Delete or rename the 'Mesh' object before running this script!"))
    
    # then import all files
    for file in glob.iglob(ObjGlob) :
        print(file)    
        importAndAnimateObj(file)
        
    # finally, set the animation bounds
    bpy.context.scene.frame_start = 0
    bpy.context.scene.frame_end = numberOfFrames  
    bpy.context.scene.frame_set(0)

def allObjects():
    """returns a list of all objects to apply an op to"""
    return [m for m in bpy.data.objects if re.match(ApplyNameRegex,m.name) is not None]

def applyMaterial():
    material = bpy.data.materials[MaterialName]
    for obj in allObjects():
        obj.data.materials.append(material)
        
def applySmooth():
    for obj in allObjects():
        framenum = frameNumFromName(obj.name)
        bpy.context.scene.frame_set(framenum)
        bpy.ops.object.select_name(name=obj.name)
        bpy.ops.object.shade_smooth()
        
def applySubdiv():
    for obj in allObjects():        
        obj = bpy.data.objects[obj.name]
        framenum = frameNumFromName(obj.name)
        bpy.context.scene.frame_set(framenum)
        print("%d "%framenum,end=" ")
        sys.stdout.flush()
            
        bpy.context.scene.frame_set(framenum)            
        bpy.context.scene.objects.active = obj
        
        # add modifier
        try:
            bpy.ops.object.modifier_add(type='SUBSURF')
            mod = obj.modifiers[-1]
            mod.levels = ViewSubdiv
            mod.render_levels = RenderSubdiv
        except TypeError:
            pass
        
if Mode==IMPORT:
    importAndAnimate()
elif Mode==APPLY_MATERIAL:
    applyMaterial()
elif Mode==APPLY_SMOOTH:
    applySmooth()
elif Mode==APPLY_SUBDIV:
    applySubdiv()
elif Mode==APPLY_ALL:
    applyMaterial()
    applySmooth()
    applySubdiv()
