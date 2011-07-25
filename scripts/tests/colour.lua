--[[
	Vertex colour
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
	local sc = 4
	local perturb = function(x,y,z) return fg.vec3(fg.noise(sc*x+sc*fgu.t,sc*y,sc*z),fg.noise(sc*x,sc*y+sc*fgu.t,sc*z),fg.noise(sc*x,sc*y,sc*z+sc*fgu.t)) end
	
	vs = m:selectAllVertices()	
	for v in vs.all do
		col = v:getColour()
		p = perturb(col.x,col.y,col.z)
		v:setColour(col+p*rnd(0.05))
	end
end

