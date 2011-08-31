--[[
	Experiment:
	- Extrude a region
	- Refine the cap boundary with edge split
	- And make cap boundary circular
--]]

module(...,package.seeall)

require 'table'
require 'fgx.mesh'
require 'fgx.extrude'
require 'fgx.pos'
require 'fgx.nloop'
require 'fgx.math'

local exsubcirc -- function

function setup()
	local m = fg.mesh.primitives.icosahedron()
	m:smoothSubdivide(1)
	fgu:addMesh(m)
	
	local v = m:selectRandomVertex()
	exsubcirc(m,v,.1,16)	
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
	while numEdgesToSplit>0 do
		-- get the cap (may have changed since last loop)					
		cap = fgx.nloop.loopp(v)
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
	end	
	
	-- move all vertices to an equal distance from the center
	table.foreachi(cap, function(i,p)
		local pc = fg.pos(p)
		pc:flipV()
		pc:flipE()
		pc.v.p = v.p + normalise(pc.v.p - v.p)*avgRadius
	end)
end

function update(dt) end

--[[
	local v = m:selectRandomVertex()
	local f = v:getAdjacentFace()
	local pos = fg.pos(f,v)
	fg.splitEdge(m,pos)
--]]
	
