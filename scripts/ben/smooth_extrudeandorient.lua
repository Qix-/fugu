--[[
	test
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
local meshops = require 'fgx.meshops'

local smoothGrowths -- object which performs the smooth growth
local newSmoothGrowth -- constructor for object
local nextvert 
local n, m
local vertices, vertex

local function debug(msg)
	print(msg)
end

function setup()
	-- here we are creating a new mesh, an icosahedron..
	-- we can modify the script and press "RELOAD" to re-run it...
	-- m = fg.mesh.primitives.icosahedron()
  	-- m = fg.mesh.primitives.dodecahedron()  	
  	-- m = fg.mesh.primitives.octahedron()
  	m = fg.mesh.primitives.cube()
  	m:smoothSubdivide(2)
  	
  	--[[
  	p.cube(),
		p.sphere(),
		p.icosahedron(),
		p.tetrahedron(),
		p.dodecahedron(),
		p.octahedron(), 
		-- p.hexahedron(), 
		p.cone(1,0.1,32),
		p.cylinder(32)
  	
  	--]]
	
	n = fg.meshnode(m)
	fgu:add(n)
	
	vertices = fgx.mesh.vertexlist(m)
	for _,v in ipairs(vertices) do
		v:setUV(0,0)
	end	
	
	smoothGrowths = {}
	for i=1,24 do
		smoothGrowths[i] = newSmoothGrowth(m,vertices[i])
	end		
end

function update(dt)
	for i,s in ipairs(smoothGrowths) do		
		if s~=nil then
			local growing = s:update(dt)
			if (not growing) then 
				s = nil
			end
		end
	end
	--[[		
	elseif #vertices>0 then
		vertex = nextvert(m)
		smoothGrowth = newSmoothGrowth(m,vertex)
	end
	--]]
end -- empty

-- get the next random vertex from vertices,
--[[
nextvert = function(m)
	if #vertices==0 then return nil end 
	local v = vertices[math.ceil(random()*#vertices)]
	local all = {} -- fgx.nloop.loopv(v)
	table.insert(all,v)
	
	for _,u in ipairs(all) do
		i,_ = table.find(vertices, function(x) return u==x end)
		if i then table.remove(vertices, i)	end
	end
	
	return v	
end
--]]

newSmoothGrowth = function(m,v)
	local obj = {
		m=m,
		v=v,		
		
		initialPosition = v.p,
		initialNormal = v.n,
		
		INSET = .8, -- scale on first inset 
		SPEED = .7,
		CIRC_SPEED = 4, -- speed the cap becomes circular
		FL_SPEED = .3, --.1, -- speed the cap flattens
		PULLDIST = .5, -- distance to pull the leading vertex
		NUMSEGS = 6,
		MAX_DETAIL = 16, -- maximum number of vertices around a cap
		detailF = 	function(dt,ds) 
						return ds 
					end, -- determines the detail at a segment (* max detail)		
		shrinkF = 	function(dt,ds) 
						return math.max(.8,sqr((1-ds))) 
					end, -- radius multiplier 
		
		-- the goal point for the tentacle
		goalF = 	function(self,dt,ds) 
						return self.initialPosition + vec3(0,100,0) -- self.initialNormal*100 
					end,
		
		segs = 1,
		AR = 0, -- average radius of first fan
		pullDir = 1, -- direction to pull (*v.n)
		-- timeWarp, affect the perception of time
		timeWarp = function(dt,ds) return dt*math.exp(ds*ds) end,
				
		time=0,
		state="waiting",
		nextState="pull",
		stateChange=.001}		
		
	-- calculate the avg radius of the loop around v 
	-- this is used to scale the growth appropriately	
	local ar = 0
	local vloop = fgx.nloop.loopv(v)
	for _,n in ipairs(vloop) do
		ar = ar + distance(n.p,v.p)
	end
	ar = ar/#vloop
	obj.AR = ar
	obj.PULLDIST = obj.PULLDIST*obj.AR
				
	-- update
	obj.update = function(self,dt)
		local ds = self.segs/self.NUMSEGS
		
		local dt = self.timeWarp(dt,ds)		
		self.time = self.time + dt
		
		local startColour = lerp(vec3(1,1,1),vec3(.2,.6,1),self.segs/self.NUMSEGS)	
		local targetColour = lerp(vec3(1,1,1),vec3(.2,.6,1),(self.segs+1)/self.NUMSEGS)
		
		if (self.state=="waiting") then
			debug("w")
			if (self.stateChange <= self.time) then
				self.state = self.nextState
			end
		elseif (self.state=="inset") then
			debug("i")
			meshops.inset(self.m,self.v,self.INSET)
			self.state = "waiting"
			self.nextState = "pull"
			self.stateChange = self.time + 1
		elseif (self.state=="pull") then 
			debug("p")
			
			if self.pullDist==nil then
				self.pullDist = 0
			end	
			
			local dist = self.SPEED*dt
			local goal = self:goalF(dt,ds)
			
			-- turn normal towards goal
			local dir = normalise(goal - self.v.p)
			local correctedNormal = slerp(self.initialNormal,dir,ds)	
			-- print(self.v.n,dir,ds*.01,correctedNormal)		
			local pullvec = correctedNormal*dist*self.pullDir
			self.v.p = self.v.p + pullvec
			
			-- adjust the outer loop on the cap
			-- so it grows in the normal, 
			-- is oriented towards the normal,
			-- reaches the right scale, 
			-- and becomes circular
			if (self.cap) then
				-- the cap poses couldve been changed
				-- TODO: try to cache this..
				self.cap = fgx.nloop.loopp(v)				
				
				local t = self.pullDist/self.PULLDIST -- amount done
				self.v:setColour(lerp(startColour,targetColour,t))
				self.v:setUV(ds,0)
				local outer = fgx.pos.capov(self.cap)

				-- compute current center
				local center = vec3(0,0,0)
				for i,ov in ipairs(outer) do
					center = center + ov.p
				end
				center = center/#outer
				
				-- move the cap in the primary growth direction
				-- towards the self.v
				-- local pgd = normalise(self.v.p - center)
				local pgd = correctedNormal
				local tp = self.v.p + pgd -- target point to align to..
				local cv = normalise(tp - center)				
				local sqrlcv = sqr(length(cv))
				
				center = vec3(0,0,0) -- reset, to calculate new center
				maxdist = 0 -- max distance shifted during orientation			
				for i,ov in ipairs(outer) do
					-- shift to align to direction					
					local ovv = tp - ov.p
					local ovt = sqrlcv/dot(ovv,cv)					
					ov.p = ov.p + ovv*(1-ovt)
					
					local dist = length(ovv*(1-ovt))
					if (dist > maxdist) then maxdist = dist end					
									
					-- ov.p = ov.p + primaryGrowthDirection*dist					
					-- ov.p = ov.p - pgd*dot(self.v.p-ov.p,pgd)
					
					--[[
					local dirToV = normalise(self.v.p-ov.p)
					local flattendist = ds*dot(dirToV,correctedNormal)
					ov.p = ov.p - dirToV*flattendist*self.FL_SPEED
					-- ov.p = ov.p + pullvec
					--]]

					-- (dist + math.max(self.FL_SPEED*dt*flattendir,flattendist))
					-- ov.p = ov.p + self.v.n*dist
					ov:setColour(self.v:getColour())
					ov:setUV(ds,0)
					center = center + ov.p
				end
				center = center/#outer
				
				-- now shift the remaining amount
				if (maxdist < dist) then
					local rest = dist - maxdist
					for i,ov in ipairs(outer) do
						ov.p = ov.p + pgd*rest
					end
					center = center + pgd*rest
				end
		
				--[[
				-- adjust the cap verts to make 
				-- them more circular
				for i,ov in ipairs(outer) do
					-- current radius, start radius
					local cr = distance(ov.p,center)
					local sr = self.capRadii[i]
					local t
					if (self.capAvgRadius-sr==0) then
						t = 1
					else
						t = (cr - sr) / (self.capAvgRadius - sr)
					end
					local tr = math.min(1, t + self.CIRC_SPEED*dt) 
					local d = normalise(ov.p-center)
					ov.p =center + d*lerp(sr,self.capAvgRadius,tr)
				end
				--]]
			end			

			self.pullDist = self.pullDist + dist			
			if (self.pullDist > self.PULLDIST) then
				self.segs = self.segs + 1				
				if (self.segs <= self.NUMSEGS) then
					if (self.pullDir > 0) then					
						self.state = "insetagain"
					end						
				else
					self.state = "done"
				end
			end		
		elseif (self.state=="insetagain") then
			debug("ia")
			-- inset a tiny bit and store the cap for subsequent pulls
			-- also add more verts to the cap if necessary
			-- depending on INC_DETAIL_SPEED	
								
			local insetVal = .99
			if self.cap==nil then			
				insetVal = self.INSET
			end
			-- obj:insetCap(insetVal,startColour,{ds,0})
			obj:insetAndRefineCap(insetVal, startColour, {ds,0}, self.detailF(dt,ds)*self.MAX_DETAIL)
			self.state = "pull"
			self.pullDist = nil --reset
			self.pullDir = 1	
		elseif (self.state=="done") then 
			debug("d")
			-- makeSpore(self.v.p,self.v.n,self.AR/3)
			return false -- finished!
		end			
		 
		return true
	end
	
	obj.insetCap = function(self, amount, col, uv)
		self.cap = meshops.inset(self.m,self.v,amount)
		-- compute the avg radius of this cap so we can circulise it
		-- also store the current radius of each outer vert
		self.capRadii = {}
		local outer = fgx.pos.capov(self.cap)
		local center = vec3(0,0,0)
		for i,ov in ipairs(outer) do
			center = center + ov.p
			ov:setColour(col)
			ov:setUV(uv[1],uv[2])			
		end
		center = center/#outer
		local avgRadius = 0
		for i,ov in ipairs(outer) do				
			local r = distance(ov.p,center)
			self.capRadii[#self.capRadii+1] = r
			avgRadius = avgRadius + r
		end
		avgRadius = avgRadius/#outer
		self.capAvgRadius = avgRadius		
	end
	
	obj.insetAndRefineCap = function(self, amount, col, uv, totalNumEdges)
		self.cap = meshops.inset(self.m,self.v,amount)
		self.capRadii = {}
		
		local cap = self.cap
		local v = self.v
		-- compute the avg radius of this cap so we can circulise it
		-- also store the current radius of each outer vert
		
		-- iterate around and identify the edges to split
		-- split the longest ones first
		local avgRadius = 0
		local numEdgesToSplit = totalNumEdges - #cap
		if numEdgesToSplit<=0 then
			-- compute avg radius 
			table.foreachi(cap, function(i,p)
				local pc = fg.pos(p)
				pc:flipV()
				avgRadius = avgRadius + distance(pc.v.p,self.v.p)
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
				avgRadius = avgRadius + distance(pc.v.p,self.v.p)
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
		
		-- get the cap (may have changed since last loop)					
		cap = fgx.nloop.loopp(v)
			
		-- recompute average radius, and set col, uv
		self.cap = cap
		-- print("#cap",#self.cap)
		local outer = fgx.pos.capov(self.cap)
		local center = vec3(0,0,0)
		for i,ov in ipairs(outer) do
			center = center + ov.p
			ov:setColour(col)
			ov:setUV(uv[1],uv[2])			
		end
		center = center/#outer
		local avgRadius = 0
		for i,ov in ipairs(outer) do				
			local r = distance(ov.p,center)
			self.capRadii[#self.capRadii+1] = r
			avgRadius = avgRadius + r
		end
		avgRadius = avgRadius/#outer
		self.capAvgRadius = avgRadius*self.shrinkF(dt,self.segs/self.NUMSEGS)	
	end
	
	return obj
end