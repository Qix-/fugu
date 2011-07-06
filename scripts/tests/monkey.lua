--[[
	Tests loading a mesh
--]]

module(...,package.seeall)

local m
function setup()
	m = fg.mesh.load("..\\assets\\suzanne.obj")
	fgu:addMesh(m)	
end

-- do nothing
function update(dt) end


