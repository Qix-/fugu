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
	local perturb = function(x,y,z) return noise(x,y,z+fgu.t) end
	
	vs = m:selectAllVertices()	
	for v in vs.all do
		pos = v:getPos()
		p = perturb(pos.x,pos.y,pos.z)
		v:setPos(pos.x + p*rnd(.01), pos.y + p*rnd(.01), pos.z + p*rnd(.01))
	end
end

