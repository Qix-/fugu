module(...,package.seeall)

local m

function setup()
	m = sphere()
	fgu:add(meshnode(m))
end

function update(dt)
each(vertexlist(m), 
		function(v)
		v.p = v.p + vec3(0,0.01*sin(v.p.x+fgu.t),0)
		end
)		
end