########################################
# Bens script for:
# - repeating an animation N times
# 
# Usage: set the USER-DEFINED vars then run the script.
#
########################################

########## USER-DEFINED VARS ###########

FrameSkip = 100 # frame n will be visible at n,n+frameskip,n+2*frameskip, etc...
Repeat = 5 # number of repeats (so total frames after running this will be Frameskip*Repeat)

# ApplyNameRegex: All objects in Blender matching 
# this will get applied to (in python regex form)
# must end in numbers (e.g., have [0-9]+ at the 
# end of the regex)
ApplyNameRegex = "bencyl.*[0-9]+"


########## script starts here ##########

import bpy, glob, sys, os.path, re

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

def allObjects():
    """returns a list of all objects to apply an op to"""
    return [m for m in bpy.data.objects if re.match(ApplyNameRegex,m.name) is not None]

def applyRepeat():
    for obj in allObjects():
        framenum = frameNumFromName(obj.name)
        for i in range(Repeat):
            animate(framenum + i*FrameSkip,obj)
    bpy.context.scene.frame_end = Repeat*FrameSkip-1
    bpy.context.scene.frame_set(0)

applyRepeat()