--[[
	Demonstrates the vertex loop function loopv to retrieve a list of vertices
	surrounding a given vertex.
--]]

module(...,package.seeall)

local m = nil
function setup()
	m = sphere()	
	fgu:add(meshnode(m))	
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
	local col = random_vec3()
	local v = m:select_random_vertex()
	v.c = vec3(1,1,1)-col
	each(loopv(v), function (pv)	
		pv:setColour(col)
	end)
end