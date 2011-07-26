--[[
	Mesh primitives, and changing mesh of a node
--]]

module(...,package.seeall)

require "table"

local meshes = {}
local n = nil
local time = 0

local index = 1

function setup()
	local p = fg.mesh.primitives
	
	meshes = {
		p.sphere(),
		p.icosahedron(),
		p.tetrahedron(),
		p.dodecahedron(),
		p.octahedron(), 
		p.hexahedron(), 
		p.cone(1,0.1,3,32),
		p.cylinder(32,32)
	}
		
	n = fg.meshnode(meshes[1])
	fgu:add(n)	
end

function update(dt)
	time = time + dt
	if (time > .5) then		
		-- cycle through the meshes
		index = index + 1
		if (index > #meshes) then index = 1 end		
		n:setMesh(meshes[index])		
		time = 0		
	end
end

