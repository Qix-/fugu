--[[
	Texture coordinates
--]]

module(...,package.seeall)

require "table"

local n = nil
local time = 0

local index = 1

function convertToTable(vtxSet)
	local t = {}
	for v in vtxSet.all do
		table.insert(t,v)
	end
	return t
end

function setup()
	local p = fg.mesh.primitives
		
	local m = p.sphere()
		
	table.foreach(convertToTable(m:selectAllVertices()), 
		function(i,v)
			local p = v:getPos()
			v:setUV(2*p.x-1,2*p.y-1 + random(-.1,.1))
		end
	)	
		
	n = fg.meshnode(m)
	fgu:add(n)
end

function update(dt) end

