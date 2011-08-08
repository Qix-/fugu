--[[
	Extrusions.
	Demonstrates the use of a self updating vertex proxy (fg::VertexProxy)
--]]

module(...,package.seeall)

require "table"

local m = nil -- the mesh
local v = nil -- a vertex selected randomly at the start
local time = 0
local ex = 0 -- num extrusions 
local maxEx = 5 

function randomElement(t)
	return t[math.floor(fg.random(1,#t+1))]	
end

function convertToTable(vtxSet)
	local t = {}
	for v in vtxSet.all do
		table.insert(t,v)
	end
	return t
end

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
		fg.extrude(m,v,1)
		time = 0
		ex = ex + 1
		
		if (ex > maxEx) then
			verts = convertToTable(fg.getVerticesAtDistance(m,v,2))
			v = randomElement(verts)
			ex = 0
		end
	end
end

