--[[
	extrusion tests
--]]

module(...,package.seeall)

require 'fgx.mesh'
require 'fgx.extrude'
require 'fgx.pos'
require 'fgx.math'
local tr = require 'fgx.transform'
local T = tr.T
local R = tr.R
local S = tr.S

--local myExtrude
local n -- mesh node

function setup()
	local m = fg.mesh.primitives.icosahedron()	
	m:subdivide(2)
	n = fg.meshnode(m)
	fgu:add(n)
		
	-- select the first vertex
	local verts = fgx.mesh.vertexlist(m)
	
	for i=1,#verts,3 do
		local v = verts[i]
		myExtrude(m,v,3,
			function(i,cv)
				local up = vec3(0,1,0)
				local n = cv.n
				n = normalise(lerp(n,up,.2))
				return T(n*.5)*T(cv.p)*S(.5)*R(math.pi/4,vec3(.3,1,0))*T(-cv.p)
			end
		)
	end
end

-- extrude a vertex n times, applying the 
-- transform function tf to each new set of faces 
function myExtrude(m,v,n,tf)
	for i=1,n do
		local cap = fgx.extrude.extrude(m,v,v.n,0.01)	
		tr.transformf(cap,tf(i,v))
		-- recompute normals of close faces and vertices
		local nearverts = fgx.mesh.nearbyv(m,v,1)
		local nearfaces = fgx.mesh.tofaces(nearverts)
		for _,f in ipairs(nearfaces) do f:calculateNormal() end
		for _,v in ipairs(nearverts) do v:calculateNormal() end				
		-- m:sync() -- recompute all normals (slow!)
	end
end

local ticker = 0
function update(dt)
	ticker = ticker + dt
	if (ticker>.1) then
		-- makeMesh(fgu.t/10,lerp(0,1,clamp(0,1,fgu.t/10)))
		ticker = 0
	end	
end

