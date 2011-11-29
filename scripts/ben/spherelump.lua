--[[
	Grow a sphere lump out of a mesh
--]]

module(...,package.seeall)

require 'table'
require 'fgx.mesh'
require 'fgx.extrude'
require 'fgx.pos'
require 'fgx.nloop'
require 'fgx.math'
require 'fgx.transform'

local exsubcirc, spherelump -- function
local n

function setup()
	local m = fg.mesh.primitives.icosahedron()
	m:smoothSubdivide(2)
	n = fg.meshnode(m)
	fgu:add(n)
	
	local v = m:selectRandomVertex()
	spherelump(m,v,.3,8,12)
end

spherelump = function(m,v,sz,latsegs,longsegs)
	local eps = 0.3
	-- first do a pinch
	local cap = exsubcirc(m,v,sz/100,longsegs)
	fgx.transform.scalef(cap,v.p,eps)
		
	-- then start making the sphere
	local dd = sz/latsegs
	local cs = eps -- current scale
	for i=1,latsegs do
		cap = exsubcirc(m,v,dd,longsegs)
		
		-- scale so it follows a spherical profile
		local x = math.abs(2*(i-1)/(latsegs-1)-1)
		local abs = eps + sqrt(1-x*x) * sz -- absolute size of cap		 
		print(i,x,abs,cs)
		fgx.transform.scalef(cap,v.p,abs/cs)
		cs = abs
	end	
end

exsubcirc = function(m,v,d,totalNumEdges)
	-- first extrude
	local cap = fgx.extrude.extrude(m,v,v.n,d)
	
	-- then iterate around and identify the edges to split
	-- split the longest ones first
	-- also, compute the average radius of vertices
	local avgRadius = 0
	local numEdgesToSplit = totalNumEdges - #cap
	if numEdgesToSplit<=0 then
		-- compute avg radius 
		table.foreachi(cap, function(i,p)
			local pc = fg.pos(p)
			pc:flipV()
			avgRadius = avgRadius + distance(pc.v.p,v.p)
		end)
		avgRadius = avgRadius / #cap
	end
	
	if (numEdgesToSplit>0) then cap = fgx.nloop.loopp(v) end	
	while numEdgesToSplit>0 do		
		-- reset average radius			
		avgRadius = 0
		-- build a list of edges {pos,len}		
		local edges = {}
		table.foreachi(cap, function(i,p)
			local pc = fg.pos(p)
			pc:flipV()
			pc:flipE()
			avgRadius = avgRadius + distance(pc.v.p,v.p)
			local other = fg.pos(pc)
			other:flipV()
			table.insert(edges,{pc,distance(other.v.p,pc.v.p)})
		end)
		avgRadius = avgRadius / #cap
		
		-- sort list by edge length, so longest is first
		table.sort(edges, function(a,b) return a[2]>b[2] end)
		
		-- finally split the longest (i.e., first)		
		fg.splitEdge(m,edges[1][1])
		
		-- decrement counter		
		numEdgesToSplit = numEdgesToSplit - 1
		
		-- get the cap (may have changed since last loop)					
		cap = fgx.nloop.loopp(v)
	end	
	
	
	-- move all vertices to an equal distance from the center
	table.foreachi(cap, function(i,p)
		local pc = fg.pos(p)
		pc:flipV()
		pc:flipE()
		pc.v.p = v.p + normalise(pc.v.p - v.p)*avgRadius
	end)
	
	return cap
end

function update(dt) end

--[[
	local v = m:selectRandomVertex()
	local f = v:getAdjacentFace()
	local pos = fg.pos(f,v)
	fg.splitEdge(m,pos)
--]]
	
