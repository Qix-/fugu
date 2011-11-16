--[[
	Demonstrates vertex colours.	
--]]

module(...,package.seeall)

local m, vs, col
function setup()
	m = sphere()
	fgu:add(meshnode(m))
	vs = vertexlist(m)
end

function update(dt)		
	each(vs, function(v)
		pos = v:getPos()
		v.c = col(pos.x,pos.y,pos.z,fgu.t)
		end)
end

col = function(x,y,z,t) 
	return vec3(
		noise(x+sin(t),y,z),
		noise(x,y+t,z),
		noise(x,y,z+t)) 
end