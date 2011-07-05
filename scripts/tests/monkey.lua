--[[
	Tests loading a mesh
--]]

module(...,package.seeall)

local m
function setup()
	m = fg.mesh.load("C:\\Users\\ben\\Documents\\blender\\suzanne.obj")
	fgu:addMesh(m)	
end

-- do nothing
function update(dt) end


