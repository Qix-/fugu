--[[
	More complex extrusions, using the extended version of extrude
--]]

module(...,package.seeall)

local m = nil -- the mesh
local v = nil -- a vertex selected randomly at the start
local time = 0
local ex = 0 -- num extrusions 
local maxEx = 20 

function setup()
	m = fg.mesh.primitives.sphere()	
	fgu:addMesh(m)
	
	-- select a random vertex
	v = m:selectRandomVertex()
end

function update(dt)
	time = time + dt
	if (time > .1) then		
		fg.extrude(m,v,1,v:getN(),.2,-.1)
		time = 0
		ex = ex + 1
		
		if (ex > maxEx) then
			v = m:selectRandomVertex()
			ex = 0
		end
	end
end

