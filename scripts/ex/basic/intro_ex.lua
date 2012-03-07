--[[
Welcome to fugu!
	
Fugu lets you create 3D animated forms by writing code in a language called Lua. 

This file is an introduction to fugu. It should get you up and running in no time. (Font too small? Press Ctrl-Plus (or Cmd-Plus) to embiggen it.)

The interface
-------------
* This is the code pane. It allows you to edit and make scripts. You can have more than one script open at a time. Switch between them using the tabs.
* To the right is the view pane. It shows the current form. You can left-click and drag to rotate. Right-click and draw to zoom (or ctrl-click on mac). And ctrl+left-click and drag to move (cmd-click on mac).
* The console lies below. It informs you if there are any errors with the script.
* The buttons above, from left to right are:
  [Activate] Select the current script as the one to run. An icon appears in the tab of the currently active script.
  [Restart] Restart the script
  [Play/Pause] Play and pause the script (Press it now to run this script..)
  [Step] Step forward one frame at a time.
  The remaining buttons affect how the mesh is shown in the view pane. The last four buttons allow you to smooth and subdivide the mesh (beware: the more subdivisions the slower fugu will be.)

Fugu Scripts
------------
There are many example scripts in File->Examples that demonstrate the different capabilities of fugu. This introduction script is documented heavily to explain the fundamentals of fugu, so be sure to read it. when the comments are removed, the script is quite succinct.

I hope you enjoy Fugu! But please note, it is alpha software, and so may contain bugs and crash from time to time. If you encounter a problem, please report it back so we can improve the software. (Help->Fugu Website->Discussion)

- Ben Porter
--]]

-- Firstly, this line is a comment. Any line beginning with "--" won't be run by fugu. You can also do block comments like the one above by surrouding multiple lines with "--[[" and "--]]".

-- Every fugu script is a Lua module. Thus every fugu script should start with the following line:
module(...,package.seeall)

-- These variables are local to the module, and allow us to access them from different functions
local the_mesh
local the_vertices
local get_flux
local get_colour

-- The setup function gets called every time the script is started. Usually this will create a mesh and initialise any other data the script needs.
function setup()
	-- Create a sphere mesh and call it "the_mesh"
	-- There are a number of other primitives, see the mesh category in the reference for info.
	the_mesh = sphere()
	-- Add the mesh to the fugu universe "fgu"
	fgu:add(meshnode(the_mesh))
	-- Store a list of all vertices in the mesh
	the_vertices = vertexlist(the_mesh)
end

-- The update function gets called every single frame. The parameter, dt, is the amount of time passed since the last update. In this example, we are iteratively modifying the colour and position of all the vertices in the sphere.
function update(dt)
	for i=1,#the_vertices do
		local v = the_vertices[i]
		local vp = v.p
		v.c = get_colour(vp.x,vp.y,vp.z,fgu.t)
		v.p = v.p + get_flux(vp.x,vp.y,vp.z,fgu.t)
	end
end

-- Below lies the definition of some helper functions for this script.

get_flux = function(x,y,z,t)
  local detail = 4
	x = detail*x
	y = detail*y
	z = detail*z
	return vec3(
		noise (x,y,z+t),
		noise (x+t,y,z),
		noise (x,y+t,z)
		)*0.001
end

get_colour = function(x,y,z,t) 
	return vec3(
		noise(x+sin(t),y,z),
		noise(x,y+t,z),
		noise(x,y,z+t)) 
end