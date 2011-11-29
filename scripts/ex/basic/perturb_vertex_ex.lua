--[[
	Manipulating vertices
	BP 16/11/2011
--]]

module(...,package.seeall)

local m, vs, perturb

function setup()
	m = dodecahedron()
	m:subdivide(3) -- subdivide it to get more detail
	vs = vertexlist(m)	
	fgu:add(meshnode(m))
end

function update(dt)		
	-- shift each vertex by a small amount
	each(vs, 
	function (v)
		local pos = v.p
		local p = perturb(pos.x,pos.y,pos.z, fgu.t)		
		local pv = random_vec3(0.1*dt*p)
		v.p = pos + pv
		end
	)
end

perturb = function(x,y,z,t) return noise(x,y,z+t) end
	