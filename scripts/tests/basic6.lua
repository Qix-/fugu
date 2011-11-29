--[[
	More complex extrusions, using the extended version of extrude
--]]

module(...,package.seeall)

local m = nil -- the mesh
local v = nil -- a vertex selected randomly at the start
local time = 0
local ex = 0 -- num extrusions 
local maxEx = 10 -- max extrusions 

-- returns the length and expansion at each extrusion
local exF = function(x) 
	local dx = x/maxEx
	local length = random()*0.3 + (1+dx)*(1+dx)*.5
	local expansion = -0.2*(1 - dx) -- math.sin(dx*10)*.1 * (0.01 + 1-dx)
	if (dx > .8) then
		expansion = .5
		if (dx > .9) then
			expansion = -.01
		end
	end
	return length, expansion
end 

function setup()
	m = sphere()	
	fgu:addMesh(m)
	
	-- select a random vertex
	v = m:selectRandomVertex()
end

function update(dt)
	time = time + dt
	if (time > .01) then		
		length, expansion = exF(ex)
		extrude_and_scale(m,v,v.n,length,expansion)
		time = 0
		ex = ex + 1
		
		if (ex > maxEx) then
			v = m:selectRandomVertex()
			ex = 0
		end
	end
end

