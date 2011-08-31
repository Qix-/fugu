--[[
	extrusion tests
--]]

module(...,package.seeall)

require 'table'
require 'fgx.mesh'
require 'fgx.extrude'
require 'fgx.pos'
require 'fgx.math'
local tr = require 'fgx.transform'
local T = tr.T
local R = tr.R
local Rv = tr.Rv
local S = tr.S

--local myExtrude
local n -- mesh node
local splines = {}
local NUM

function setup()
	local m = fg.mesh.primitives.icosahedron()	
	m:subdivide(2)
	n = fg.meshnode(m)
	fgu:add(n)
	
	local verts = fgx.mesh.vertexlist(m)	
	NUM = math.min(#verts,36) -- 12)
	for i=1,NUM do
		table.insert(splines, 
			{S=random(.7,.8),
			 R1=random(math.pi/7,math.pi*.6),
			 V1=normalise(vec3(random(-.4,.4),1,random(-.4,.4)))
			 })
	end
	
	makeThing(0)
end

function makeThing(foo)
	local m = fg.mesh.primitives.icosahedron()	
	m:subdivide(2)
	n:setMesh(m)
	
	local verts = fgx.mesh.vertexlist(m)
	
	
	--local oid = math.sin(foo)
	--oid = sqr(oid)
	--local pulse = .01 + oid
	local pulse
	if (foo <= 1) then 
		pulse = sqr(sqr(sqr(foo)))
	else
		pulse = math.exp(1-foo)
	end	
	
	local F = 10
	local TL = F*pulse
	local E = math.floor(TL)
	local dp = TL - E
		
	for index=1,NUM do
		local v = verts[index]					
		local sp = splines[index]
		myExtrude(m,v,E,
			function(i,cv)				
				local n = cv.n
				local di = i/E
				local L = 1/E
				local s = sp.S
				local seglength = L*pulse*((E-i)+dp)
				local axis = normalise(lerp(n,sp.V1,.7))
				return T(n*seglength/s)*T(cv.p)*S(s)*R(pulse*sp.R1,axis)*T(-cv.p)
			end
		)
	end
end

-- extrude a vertex n times, applying the 
-- transform function tf to each new set of faces 
function myExtrude(m,v,n,tf)
	for i=1,n do
		local cap = fgx.extrude.extrude(m,v,v.n,0.001)	
		tr.transformf(cap,tf(i,v))
		-- recompute normals of close faces and vertices
		local nearverts = fgx.mesh.nearbyv(m,v,2)
		local nearfaces = fgx.mesh.tofaces(nearverts)
		for _,f in ipairs(nearfaces) do f:calculateNormal() end
		for _,v in ipairs(nearverts) do v:calculateNormal() end						
		-- m:sync() -- recompute all normals (slow!)
	end
end

function update(dt)
	makeThing(fgu.t)	
end

