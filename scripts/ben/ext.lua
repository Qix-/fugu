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
	local v = fgx.mesh.vertexlist(m)[1]		
	myExtrude(m,v,4,2.0,S(.7)*R(math.pi/4,vec3(.3,1,0)))
end

-- extrude a vertex n times by amount e, applying transform t to each new set of faces 
function myExtrude(m,v,n,e,t)
	--local m = fg.mesh.primitives.icosahedron()
	--m:subdivide(1)
	--n:setMesh(m)

	-- select the first vertex
	--local v = fgx.mesh.vertexlist(m)[1]	
	
	-- rotate n towards up (crudely)
	--local up = vec3(0,1,0)
	--n = normalise(lerp(n,up,rotation))
	
	for i=1,n do 
		print(length(v.n))
		local cap = fgx.extrude.extrude(m,v,v.n,e)		
		tr.transformf(cap,T(v.p)*t*T(-v.p))
		m:sync() -- recompute normals
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

