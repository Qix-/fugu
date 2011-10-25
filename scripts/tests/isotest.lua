--[[
	A simple demo -BP	
--]]
module(...,package.seeall)

local function floober(x,y,z,t)
	local r,th,ph = polar(x,y,z)		
	local mult = 1+0.5*sqr(t);
	return r + .2*cos(ph*mult*sin(t*.73))*sin(th*(mult*.75)*cos(t*.7)) - .5
end

local function floob(x,y,z)
	return floober(x,y,z,0)
end

local res = 30
local m, n
function setup()
	m = iso(res, floob)
	n = meshnode(m)
	fgu:add(n)
end

function update(dt)
	m = iso(res, function (x,y,z) return floober(x,y,z,fgu.t) end )
	n:set_mesh(m)
end