--[[
	A thing	
--]]

module(...,package.seeall)

require "table"

local m = nil -- the mesh
local time = 0
local tentacle = nil
local sphereVerts = nil -- all verts on the initial sphere
local thing = nil

function randomElement(t)
	return t[math.floor(fg.random(1,#t+1))]	
end

function convertToTable(vtxSet)
	local t = {}
	for v in vtxSet.all do
		table.insert(t,v)
	end
	return t
end

function setup()
	m = fg.mesh.primitives.sphere()	
	fgu:addMesh(m)
	
	sphereVerts = convertToTable(m:selectAllVertices())	
	thing = new_thing(randomElement(sphereVerts)) -- m:selectRandomVertex()	 
end

function update(dt)
	time = time + dt
	if (time > 1) then
		if (thing~=nil) then
			if (not thing:update(dt)) then
				thing = nil
				thing = new_thing(randomElement(sphereVerts)) -- m:selectRandomVertex())
			end
		end
		time = 0
	end
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

function new_thing(v) -- use the global mesh m
	local obj = {
		vertex = v,
		numSegments = 15,
		totalLength = 1,
	}

	obj.update = function(self,dt)
		if (not self.vertex.valid) then 
			print("a sucker's leading vertex has become invalid!")
			return false
		end

		-- specifies the length of the extrusion, expansion, and direction
		-- as a function of u, where u is between 0 and 1
		local n = self.vertex:getN()
		n:normalise()
		function f(u)
			local len = stepFunction({{0.2,fg.random(.2,.3)},{.3,.01},{.6,-.2},{1.1,-.3}})(u)
			local exp = stepFunction({{.1,fg.random(.1,.4)},{.3,-.1},{.6,-.1},{1.1,-.2}})(u)
			local d = nil
			if (len > 0) then d = n else d = n*-1 end
			return math.abs(len)*self.totalLength/self.numSegments, exp, d
		end
		
		for k=1,self.numSegments do
			len,exp,dir = f((k-1)/(self.numSegments-1))
			fg.extrude(m,self.vertex,1,dir,len,exp)

			-- recompute the normals if needed 
			m:sync()										
		end		
		return false			

	end
	return obj
end



function new_tentacle(m,v)
	-- create initial params
	local obj = {
		mesh = m,
		vertex = v,
		attractor=.4,		
		numSegments= 7,
		time=0,
		startSegLength = .3,		
		numSubSegments = 3,
		segWidth = -0.05,		
	}
	
	-- derived params
	obj.segLengthDiff = -(obj.startSegLength-0.05) * 1./obj.numSegments
	obj.segLength = obj.startSegLength
	
	-- update function
	obj.update = function(self,dt)
		-- ATM just build the whole thing in one step
		-- self.time = self.time + dt
		
		if (not self.vertex.valid) then 
			print("a tentacle's leading vertex has become invalid!")
			return false
		end
		
		-- else, grow a thing
		local n = self.vertex:getN()
		n:normalise()
		
		for k=1,self.numSegments do
			local r = fg.vec3(fg.random(-.3,.3),1,fg.random(-.3,.3))
			r:normalise()					
			
			n = n*(1-self.attractor) + r*(self.attractor)
			n:normalise()
						
			for i=1,self.numSubSegments do
				fg.extrude(self.mesh,self.vertex,2,n,self.segLength/self.numSubSegments,self.segWidth)

			end
			self.segLength = self.segLength + self.segLengthDiff
			
			-- recompute the normals if needed 
			-- m:sync()										
		end		
		return false			
	end	
	
	-- finally return the object
	return obj
end

