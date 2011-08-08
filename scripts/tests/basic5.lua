--[[
	Extrusions.
	Demonstrates the use of a self updating vertex proxy (fg::VertexProxy)
--]]

module(...,package.seeall)

local m = nil -- the mesh
local v = nil -- a vertex selected randomly at the start
local time = 0
local ex = 0 -- num extrusions 
local maxEx = 5 

function setup()
	m = fg.mesh.primitives.sphere()	
	fgu:addMesh(m)
	
	-- select a random vertex
	v = m:selectRandomVertex()
end

function update(dt)
	time = time + dt
	if (time > .1) then
		-- extrude the same vertex over and over again		
		fg.extrude(m,v,.2)
		time = 0
		ex = ex + 1
		
		if (ex > maxEx) then
			v = m:selectRandomVertex()
			ex = 0
		end
	end
end

