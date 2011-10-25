--[[
	A simple demo -BP	
--]]
module(...,package.seeall)
 
mesh = require "fgx.mesh"
  
function vnoise(v)
	-- return a vec3 of noise
	return vec3(noise(v.x),noise(v.y),noise(v.z))
end

function pulser(t)
	return sign(sin(t))*sqr(sin(t))
end

-- returns (r,theta,phi) of p
function cartesian_to_spherical(p)
	local r = length(p)
	local theta = acos(p.z/r)
	local phi = atan2(p.y,p.x)
	return {r,theta,phi}
end

local m
local op
local sph
function setup()
	m = fg.mesh.primitives.icosahedron()
	m:smoothSubdivide(2)
	local n = fg.meshnode(m)
	n:setTransform(mat4():setRotateRad(pi/2,vec3(1,0,0)))
	fgu:add(n)	 
	
	op = {}
	sph = {}
	table.foreach(mesh.vertexlist(m), function(_,v) op[#op+1]=v.p end )		
	table.foreach(op, function(_,p) sph[#sph+1] = cartesian_to_spherical(p) end )		
end

function update(dt)
	for i,v in ipairs(mesh.vertexlist(m)) do
		local th = sph[i][2]
		local ph = sph[i][3]
		if (th > pi/2) then
			local shift = pulser(3*fgu.t+(1+sin(pi*v.p.x))*ph)*1.5*(th-pi/2)/(pi/2)
			v.p = op[i] + vec3(0,0,shift)			
			v:setColour(lerp(vec3(1,1,1),vec3(0,abs(cos(ph)),1),abs(shift/1.5)))
		end		
	end
end