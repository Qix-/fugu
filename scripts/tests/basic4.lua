--[[
	Basic extrusions
--]]

module(...,package.seeall)

require "table"

local m = {}
local time = 0

function setup()
	m = fg.mesh.primitives.sphere()	
	fgu:addMesh(m)
	
	math.randomseed(1234)
end

function update(dt)
	time = time + dt
	if (time > .01) then		
		vs = m:selectAllVertices()
		-- convert to table (TODO: implement Collections)
		local vs_t = {}
		for v in vs.all do table.insert(vs_t,v) end
		-- table.foreach(vs_t,print)		
		local numVerts = table.getn(vs_t)
		--print("numVerts:", numVerts)
		local index = math.random(numVerts)		
		--print("Index:", index)
		local v = vs_t[index]		
		fg.extrude(m,v,.8)
		time = 0
	end
end

