--[[
	Demos applying texture coordinates to a mesh.
	Make sure that Textured is enabled in fugu to see the results.
--]]

module(...,package.seeall)

local n = nil
local index = 1

function setup()
	local m = sphere()
		
	each(vertexlist(m), function(v)
		local p = v:getPos()
		v:set_uv(2*p.x-1,2*p.y-1 + random(-.1,.1))
	end)	
		
	n = meshnode(m)
	fgu:add(n)
end

function update(dt) end

