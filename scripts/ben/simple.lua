--[[
	A simple demo -BP
--]]
module(...,package.seeall)

mesh = require "fgx.mesh"

function vnoise(v)
	-- return a vec3 of noise
	return vec3(noise(v.x),noise(v.y),noise(v.z))
end

local m
local op
function setup()
	m = fg.mesh.primitives.icosahedron()
	m:smoothSubdivide(2)
	local n = fg.meshnode(m)
	n:setTransform(mat4():setScale(2))
	fgu:add(n)	
	op = {}
	table.foreach(mesh.vertexlist(m), function(_,v) op[#op+1]=v.p end )	
end

function update(dt)
	for i,v in ipairs(mesh.vertexlist(m)) do
		local t = math.sin(3*fgu.t + 3*op[i].z*op[i].x)
		v.p = op[i] + vec3(0,t,math.sin(t*.13))
	end
end