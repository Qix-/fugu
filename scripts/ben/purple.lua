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

local smoothGrowth -- object which performs the smooth growth
local newSmoothGrowth -- constructor for object
local nextvert 
local n, m
local vertices, vertex

function setup()
	m = fg.mesh.primitives.icosahedron() -- cube()
	m:smoothSubdivide(2)
	-- m:subdivide(2)
	n = fg.meshnode(m)
	fgu:add(n)
	
	vertices = fgx.mesh.vertexlist(m)
	smoothGrowth = nil
end

function update(dt)
	if smoothGrowth~=nil then
		local more = smoothGrowth:update(dt)
		if (not more) then 
			smoothGrowth = nil
		end
	elseif #vertices>0 then
		vertex = nextvert(m)
		smoothGrowth = newSmoothGrowth(m,vertex)
	end
end -- empty

-- get the next random vertex from vertices,
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

newSmoothGrowth = function(m,v)
	local obj = {
		m=m,
		v=v,
		SPEED = 1,
		PULLDIST = 0.1,
		NUMSEGS = 5,
		RADII = {1,.5,1.2,.9,.5},
		segs = 1,
				
		time=0,
		state="waiting",
		nextState="pull",
		stateChange=.01}
			
	obj.update = function(self,dt)
		self.time = self.time + dt
		
		if (self.state=="waiting") then
			if (self.stateChange <= self.time) then
				self.state = self.nextState
			end
		elseif (self.state=="inset") then
			meshops.inset(self.m,self.v,.8)
			self.state = "waiting"
			self.nextState = "pull"
			self.stateChange = self.time + 1
		elseif (self.state=="pull") then 
			if self.pullDist==nil then
				self.pullDist = 0
			end	
			local dist = self.SPEED*dt
			self.v.p = self.v.p + self.v.n*dist
			local targetColour = lerp(vec3(1,1,1),vec3(.6,.4,1),self.segs/self.NUMSEGS)
			if (self.cap) then
				-- adjust the outer loop on the cap
				-- so it grows in the normal, 
				-- reaches the right scale, 
				-- and becomes circular				
				local t = self.pullDist/self.PULLDIST -- amount done
				self.v:setColour(lerp(vec3(1,1,1),targetColour,t))
				local outer = fgx.pos.capov(self.cap)
				local center = vec3(0,0,0)
				for i,ov in ipairs(outer) do
					-- move in growing direction
					ov.p = ov.p + self.v.n*dist
					ov:setColour(lerp(vec3(1,1,1),targetColour,t))
					center = center + ov.p
				end
				center = center/#outer
				local er = self.capAvgRadius * self.RADII[self.segs]
				for i,ov in ipairs(outer) do
					-- make a bit more circular
					-- current, start, end radii
					local cr = distance(ov.p,center)
					local sr = self.capRadii[i]					
					local tr = math.max(1,self.SPEED*dt + (cr-sr)/(er-sr))
					local d = normalise(ov.p-center)
					ov.p = center + d*lerp(sr,er,tr)
					-- scale from the center some amount
					-- ov.p = lerp(center,ov.p,1.01)
				end
			end			
			self.pullDist = self.pullDist + dist			
			if (self.pullDist > self.PULLDIST) then
				self.segs = self.segs + 1
				if (self.segs < self.NUMSEGS) then
					self.state = "insetagain"
				else
					self.state = "done"
				end
			end		
		elseif (self.state=="insetagain") then
			-- inset a tiny bit and store the cap for subsequent pulls								
			self.cap = meshops.inset(self.m,self.v,.99)
			-- compute the avg radius of this cap so we can circulise it
			-- also store the current radius of each outer vert
			self.capRadii = {}
			local outer = fgx.pos.capov(self.cap)
			local center = vec3(0,0,0)
			for i,ov in ipairs(outer) do
				center = center + ov.p
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
			
			self.state = "pull"
			self.pullDist = nil --reset
		elseif (self.state=="done") then 
			return false -- finished!
		end			
		 
		return true
	end
	
	return obj
end