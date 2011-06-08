--[[
	Basic manipulation of vertex positions
--]]

module(...,package.seeall)

local m = {}
function setup()
	m = fg.mesh.primitives.icosahedron()
	-- m.subdivide(2) -- not yet implemented
	fgu:addMesh(m)
end

function update(dt)	
	vs = m:selectAllVertices()
	local foo = function() return (1-2*math.random())*dt*math.sin(fgu.t) end
	for v in vs.all do		
		v.pos.x = v.pos.x + foo()
		v.pos.y = v.pos.y + foo()
		v.pos.z = v.pos.z + foo()
	end
end

