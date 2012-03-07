--[[
	Demonstrates the isosurface primitive.
--]]

module(...,package.seeall)

local floober, floober_zero -- generating functions for the iso surface

local resolution = 14 
local m, n
function setup()
	m = iso(resolution, floober_zero)
	n = meshnode(m)
	fgu:add(n)
end

function update(dt)
	m = iso(resolution, function (x,y,z) return floober(x,y,z,fgu.t) end )
	n:set_mesh(m)
end

floober = function(x,y,z,t)
	local r,th,ph = polar(x,y,z)		
	local mult = 1+0.5*clamp(sqr(t),0,10);
	return r + .2*cos(ph*mult*sin(t*.73))*sin(th*(mult*.75)*cos(t*.7)) - .5
end

floober_zero = function(x,y,z) return floober(x,y,z,0) end