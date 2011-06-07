--[[
	Basic manipulation of vertex positions
--]]

module(...,package.seeall)

local m = {}
function setup()
	m = fg.mesh.primitives.icosahedron()
	fgu:addMesh(m)
end

-- do nothing
function update(dt)
	--[[
	for v in m:vertices() do
		print(v)
		break
	end
	--] ]	
	for v in m:vertices() do		
		-- v.pos.x = v.pos.x + (1-2*math.random())*dt
		-- v.pos.y = v.pos.y + (1-2*math.random())
		-- v.pos.z = v.pos.z+0.01 -- v.pos.z + (1-2*math.random())		
		-- v:getPosition().z = 0
	end
	--]]
end


