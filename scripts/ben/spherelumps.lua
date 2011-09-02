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
require 'fgx.util' -- table.find
meshops = require 'fgx.meshops'

local exsubcirc, spherelump, nextvert -- functions 
local n, m
local vertices

function setup()
	m = fg.mesh.primitives.dodecahedron()
	m:smoothSubdivide(1)
	n = fg.meshnode(m)
	fgu:add(n)
	
	vertices = fgx.mesh.vertexlist(m)
	
	-- lets just make one thing
	-- while #vertices>1 do table.remove(vertices,2) end	
end

local ticker = 0
local count = 1
function update(dt)
	ticker = ticker + dt
	if ticker>.5 then
		if #vertices>0 then
			count = count + 1
			local sz = .1 + count/500
			local numSegs = 4 + math.ceil(count/20)
			spherelump(m,nextvert(m),sz,numSegs,numSegs)
		end
		ticker = 0
	end
end -- empty

-- get the next random vertex from vertices, 
-- and remove it -- (NOT Anymore) and its immediate neighbours
nextvert = function(m)
	local v = vertices[math.ceil(random()*#vertices)]
	-- local all = fgx.nloop.loopv(v)
	local all = {}
	table.insert(all,v)
	
	for _,u in ipairs(all) do
		i,_ = table.find(vertices, function(x) return u==x end)
		if i then table.remove(vertices, i)	end
	end
	
	return v	
end

spherelump = function(m,v,sz,latsegs,longsegs)
	local eps = sz/(latsegs+1)
	local dd = sz/latsegs
	
	-- first do a pinch
	local cap
	local avgRadius 
	cap, avgRadius = exsubcirc(m,v,dd,longsegs)	
	fgx.transform.scalef(cap,v.p,eps/avgRadius)	
		
	-- then start making the sphere
	-- print("sz",sz,"dd",dd,"latsegs",latsegs,"longsegs",longsegs)	
	local cs = eps -- current scale
	for i=1,latsegs do
		cap, avgRadius = exsubcirc(m,v,dd,longsegs)
		
		local di = i/latsegs
		-- scale so it follows a spherical profile
		local x = math.abs(2*di-1) 
		local abs = eps + sqrt(math.max(0.0001,1-x*x)) * (sz/2) -- absolute size of cap
					
		fgx.transform.scalef(cap,v.p,abs/cs)
		cs = abs		
	end	
end

exsubcirc = function(m,v,d,totalNumEdges)
	-- extrude 
	local cap = fgx.extrude.extrude(m,v,v.n,d)
	v:calculateNormal()
	
	-- flatten the cap
	local outer = {} -- outer verts 	
	table.foreachi(cap, function(i,p)
		local pc = fg.pos(p)
		pc:flipV()		
		outer[#outer+1] = pc.v
		-- pc.v.p = pc.v.p - v.n*dot(pc.v.p-v.p,v.n)
	end)		
	meshops.flattenvl(m,outer,v.p,v.n)
	
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
		pc.f:calculateNormal()
		
		pc:flipV()
		pc:flipE()
		pc.v.p = v.p + normalise(pc.v.p - v.p)*avgRadius
	end)	
	
	return cap, avgRadius
end



--[[
	local v = m:selectRandomVertex()
	local f = v:getAdjacentFace()
	local pos = fg.pos(f,v)
	fg.splitEdge(m,pos)
--]]
	
