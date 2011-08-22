--[[
	extrusion tests
--]]

module(...,package.seeall)

require 'fgx.extrude'
require 'fgx.pos'
require 'fgx.transform'


local m = nil -- the mesh
local v = nil -- a vertex selected randomly at the start

-- returns the length and expansion at each extrusion
local exF = function(x) 
	local dx = x/maxEx
	local length = dx
	return length, 0
end 

function setup()
	m = fg.mesh.primitives.icosahedron()	
	m:smoothSubdivide(2)
	fgu:addMesh(m)
	
	-- select a random vertex
	v = m:selectRandomVertex()
	local n = v.n
	
	-- rotate n towards up (crudely) 
	local up = vec3(0,1,0)
	n = normalise(lerp(n,up,.5))	
	
	local cap
	cap = fgx.extrude.extrude(m,v,n,1)
	m:sync()
	
	--[[
	print("cap pos")
	table.foreach(cap,print)
	print("ifaces")	
	for _,f in fgx.pos.ifaces(cap) do 
		print(f) 
	end
	--]]
	
	fgx.transform.scalef(cap,v.p,2)
		
	--[[
	fg.extrude(m,v,1,n,1)
	m:sync()
	local loop = fg.getVerticesAtDistance(m,v,1) -- fg.nloop(m,v,1)
	for nv in loop.all do
		nv.p = nv.p + nv.n*.1
	end	
	--]]	
end

function update(dt) end

