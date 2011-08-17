--[[
-- Demonstrates 'requiring' an fg lib
--]]

module(...,package.seeall)

require 'fg.nring'

local m = nil
function setup()
	m = fg.mesh.primitives.sphere()	
	fgu:add(fg.meshnode(m))	
end

local ticker = 0 
function update(dt)
	ticker = ticker + dt
	if (ticker > .2) then
		doit()
		ticker = 0
	end	
end

function doit()
	local col = vec3(random(),random(),random())
	local v = m:selectRandomVertex()
	v:setColour(vec3(1,1,1)-col)
	for _,pv in ipairs(fg.nring.onering(v)) do
		pv:setColour(col)
	end	
end