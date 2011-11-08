--[[
	Generate an icosahedron and rotate it.	
--]]

module(...,package.seeall)

local n
function setup()
	local m = icosahedron()
	n = meshnode(m)
	fgu:add(n)
end

function update(dt) 
	local y_axis = vec3(0,1,0)
	n:set_transform(n:get_transform() * R(dt,y_axis))
end