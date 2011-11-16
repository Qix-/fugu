--[[
	Shows how to access face information in a mesh.
--]]

module(...,package.seeall)

local counter = 0
local faces -- faces of the base mesh
local nn = nil -- a node oriented to lie in the center of a face with its y axis pointed in the direction of the face normal 

function setup()
	local m = sphere()
	faces = facelist(m)
	fgu:add(meshnode(m))
	
	-- c is a cone with base at 0,0,0
	local c = cone(.5,0,16)
	c:apply_transform(T(0,1,0))
	c:apply_transform(S(.3,.3,.3))
	
	nn = meshnode(c)
	fgu:add(nn)
end

function update(dt)
	counter = counter + dt
	if (counter > .1) then
		-- select a random face and move the nn to the center
		local f = choose(faces)
		local centroid = (f:v(0).p + f:v(1).p + f:v(2).p)/3		
		local n = f.n
		nn:set_transform(T(centroid) * Rv(vec3(0,1,0),n))
		counter = 0
	end
end