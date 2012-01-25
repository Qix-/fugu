--[[
	Grow simple suckers on a mesh.
	BP 16/11/2011
--]]

module(...,package.seeall)

local m = nil -- the mesh
local counter = 0
local verts = nil -- stores all vertices on the sphere
local thing = nil

function setup()
	m = sphere()	
	verts = vertexlist(m)
	thing = new_thing(m,choose(verts))
	
	fgu:add(meshnode(m))
end

function update(dt)
	counter = counter + dt
	if (counter > .2) then
		if (thing~=nil) then
			if (not thing:update(dt)) then
				thing = nil
				thing = new_thing(m,choose(verts))
			end
		end
		counter = 0
	end
end

-- create a new sucker grower on mesh m at vertex v
function new_thing(m,v)
	-- create a new object 
	local obj = {
		mesh = m,
		vertex = v,
		num_segments = 8,
		total_length = 1,
	}
	
	-- defines the profile (length and expansion) of the sucker
	-- as a function of u in [0,1]
	obj.profile = function(self,u,n)
		local len = step_function({
			{0.2,random(.5,.8)},
			{.3,.01},
			{.6,-.2},
			{1.1,-.3}})(u)
		local ex = step_function({
			{.1,random(1.1,1.4)},
			{.3,0.9},
			{.6,0.9},
			{1.1,0.8}})(u)
		local d = nil
		if (len > 0) then d = n else d = n*-1 end
		return (abs(len)*self.total_length/self.num_segments), ex, d
	end
	
	-- the update function for this object just grows a sucker
	-- on the target mesh
	obj.update = function(self,dt)
		if (not self.vertex.valid) then 
			-- "a sucker's leading vertex has become invalid!"
			return false
		end
		
		local n = self.vertex:getN()
		n:normalise()
		for k=1,self.num_segments do			
			local len,ex,dir = self:profile((k-1)/(self.num_segments-1),n)
			extrude_and_scale(self.mesh,self.vertex,dir,len,ex)
			-- recompute the normals if needed
			m:sync()										
		end		
		return false
	end
	return obj
end
