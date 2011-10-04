--[[
	Demonstrates vertex colouring
--]]

module(...,package.seeall)

local m
local vs
function setup()
	m = fg.mesh.primitives.sphere()
	m:subdivide(1)
	fgu:addMesh(m)
	vs = m:selectAllVertices()	
end

function update(dt)		
	local col = function(x,y,z) 
		return vec3(noise(x+sin(fgu.t),y,z),
			noise(x,y+fgu.t,z),
			noise(x,y,z+fgu.t)) 
	end
	
	for v in vs.all do
		pos = v:getPos()
		v:setColour(col(pos.x,pos.y,pos.z))
	end
end

