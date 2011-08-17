--[[
	Tests node transforms
--]]

module(...,package.seeall)

local m -- the mesh
local thing -- manipulates the mesh
local n1, n2, n3
function setup()	
	m = fg.mesh.primitives.sphere()
	setupThing()
	
	n1 = fg.meshnode(m)
	n2 = fg.meshnode(m)
	n3 = fg.node() -- blank transform node
			
	fgu:add(n1)
	fgu:add(n2)
	fgu:add(n3)
	
	fgu:makeChildOf(n1,n3)
	fgu:makeChildOf(n3,n2)	
	
	n2:setTransform(mat4():setTranslate(3,0,0))		
end


function update(dt) 
	y = math.sin(.5*fgu.t * math.pi)
	y = y*y*math.abs(y)
	n1:setTransform(mat4():setTranslate(0,y,0))
	n3:setTransform(mat4():setRotateRad(fgu.t,math.cos(fgu.t),1+math.sin(fgu.t),1))
	
	updateThing(dt)
end


--[[

 THE THING LURKS BELOW THIS COMMENT!

--]]

local sphereVerts = nil -- all verts on the initial sphere

local suckers = {}

function randomElement(t)
	return t[math.floor(random(1,#t+1))]	
end

function convertToTable(vtxSet)
	local t = {}
	for v in vtxSet.all do
		table.insert(t,v)
	end
	return t
end

function setupThing()	
	sphereVerts = convertToTable(m:selectAllVertices())

	for i=1,20 do
		local el = sphereVerts[i] -- randomElement(sphereVerts)
		table.insert(suckers,new_thing(el,random(0,10),random(1,3)))
	end
end

function updateThing(dt)
	table.foreachi(suckers, 
		function(i,thing)
			if (thing~=nil) then
				if (not thing:update(dt)) then suckers[i] = nil end		
			end
		end)

end

-- Give step function a list of (x,t) pairs (0<=x<=1)
-- and it returns a function which maps from 0<x<1 to t
function stepFunction(list)
	local f = function(x)
		for i,xt in ipairs(list) do
			if (x < xt[1]) then return xt[2] end
		end -- for
		-- else
		return list[#list][2]
	end -- f
	return f
end

function new_thing(v,p,s) -- use the global mesh m
	local obj = {
		vertex = v,
		phase = p,
		speed = s,
		numSegments = 5,
		totalLength = 1,
		hasGrown = false, -- has this sucker gone through the grow phase?
		edgeRings = {} -- keep track of the rings of vertices so we can animate them
	}

	obj.growSucker = function(self,dt)
		-- specifies the length of the extrusion, expansion, and direction
		-- as a function of u, where u is between 0 and 1
		local n = self.vertex:getN()
		n:normalise()
		function f(u)
			local len = stepFunction({{0.5,random(.6,.8)},{.6,.01},{.7,-.2},{1.1,-.1}})(u)
			local exp = stepFunction({{.1,random(.1,.4)},{.3,-.01},{.6,-.6},{1.1,-.1}})(u)
			local d = nil
			if (len > 0) then d = n else d = n*-1 end
			return math.abs(len)*self.totalLength/self.numSegments, exp, d
		end
		
		for k=1,self.numSegments do
			len,exp,dir = f((k-1)/(self.numSegments-1))
			fg.extrude(m,self.vertex,1,dir,len,exp)
			
			-- store the center, edge ring, and original edge ring positions			
			local ring = convertToTable(fg.getVerticesAtDistance(m,self.vertex,1))

			local ringDP = {}
			table.foreachi(ring,function(i,r) 			
				table.insert(ringDP,r:getPos() - self.vertex:getPos()) 
			end)
			table.insert(self.edgeRings,
				{center=self.vertex:getPos(),
				ring=ring,
				ringDP=ringDP})
			-- recompute the normals if needed
			m:sync()										
		end	

		self.hasGrown = true
	end

	obj.animateSucker = function(self,dt)
		table.foreachi(self.edgeRings, function(i, er)
			for i=1,#er.ring do
				if (er.ring[i].valid) then
					local s = math.sin(self.speed*2*fgu.t + self.phase)
					s = math.pow(s,4)
					er.ring[i]:setPos(er.center + er.ringDP[i]*(1+0.5*s))
				end
			end
		end)
		m:sync()
	end

	obj.update = function(self,dt)
		if (not self.vertex.valid) then 
			print("a sucker's leading vertex has become invalid!")
			return false
		end

		if (not self.hasGrown) then 
			self:growSucker() 
		else 
			self:animateSucker()
		end

		return true

	end
	return obj
end

