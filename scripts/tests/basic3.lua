--[[
	Basic manipulation of vertex positions
--]]

module(...,package.seeall)

local m = {}
function setup()
	m = fg.mesh.primitives.icosahedron()
	m:subdivide(4) -- subdivide it to get more detail
	fgu:addMesh(m)
end

function update(dt)		
	local rnd = function(max) return max*math.random() end	
	local perturb = function(x,y,z) return fg.noise(x,y,z+fgu.t) end
	
	vs = m:selectAllVertices()	
	for v in vs.all do
		p = perturb(v.pos.x,v.pos.y,v.pos.z)
		v.pos.x = v.pos.x + p*rnd(.01)
		v.pos.y = v.pos.y + p*rnd(.01)
		v.pos.z = v.pos.z + p*rnd(.01)
	end
end

