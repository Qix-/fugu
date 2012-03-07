--[[
	Demonstrates the different primitives in fugu.
	BP 16/11/2011	
--]]

module(...,package.seeall)

local meshes = {}
local n = nil
local counter = 0
local index = 1

function setup()
	meshes = {
		cube(),
		sphere(),
		icosahedron(),
		tetrahedron(),
		dodecahedron(),
		octahedron(), 		
		cone(1,0.1,32),
		cylinder(32),
		-- isosurface
		iso(32, function(x,y,z)
			return x*x + noise(x,y,z) + z*z - .25
		end)
	}
		
	n = meshnode(meshes[1])
	fgu:add(n)
end

function update(dt)
	counter = counter + dt
	if (counter > .5) then		
		-- cycle through the meshes
		index = index + 1
		if (index > #meshes) then index = 1 end		
		n:set_mesh(meshes[index])		
		counter = 0		
	end
end

