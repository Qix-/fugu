--[[
	Accessing faces
--]]

module(...,package.seeall)

require "table"

local time = 0
local faces = nil -- all faces of the initial mesh
local nn = nil -- a node oriented to lie in the center of a face with its y axis pointed in the direction of the face normal 

function randomElement(t)
	return t[math.floor(random(1,#t+1))]	
end

function toTable(set)
	local t = {}
	for v in set.all do
		table.insert(t,v)
	end
	return t
end

function setup()
	local m = fg.mesh.primitives.sphere()
	faces = toTable(m:selectAllFaces())
	fgu:add(fg.meshnode(m))
	
	-- c is a cone with base at 0,0,0
	local c = fg.mesh.primitives.cone(.5,0,16)
	c:applyTransform(mat4():setTranslate(0,1,0))
	c:applyTransform(mat4():setScale(.3,.3,.3))
	
	nn = fg.meshnode(c)
	fgu:add(nn)
end

function update(dt)
	time = time + dt
	if (time > .5) then
		-- select a random face and move the nn to the center
		local f = randomElement(faces)
		local centroid = (f:v(0).p + f:v(1).p + f:v(2).p)/3		
		local n = f.n
		nn:setTransform(mat4():setTranslate(centroid) * mat4():setRotate(vec3(0,1,0),n))
		time = 0
	end
end