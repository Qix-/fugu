--[[
 Simple example of growing spikes on a surface.
 BP 05-03-2012 
--]]

module(...,package.seeall)

local m, spike, new_spike, next_vert

function setup()
	m = icosahedron()
	m:smooth_subdivide(3)
	local n = meshnode(m)
	fgu:add(n)
	
	vertices = {}
	local all_vertices = vertexlist(m)
	while #all_vertices>0 do 
		vertices[#vertices+1] = next_vert(all_vertices)
	end
	spike = nil
end

function update(dt)
	
		if (spike~=nil) then
			local continue = spike:update(dt)
			if (not continue) then 
				spike=nil 
			end
		end
		if (spike==nil and #vertices>0) then
			local v = vertices[#vertices]			
			remove(vertices,#vertices)
			spike = new_spike(m,v)
		end
	
end

-- get the next random vertex from vertices, 
-- and remove its immediate neighbours
next_vert = function(vertices)
	if #vertices==0 then return nil end 
	local v = vertices[1] -- just choose the first one
	local neighbours = loopv(v)
	insert(neighbours,v)
	each(neighbours, 
		function(u)
			i,_ = find(vertices, function(x) return u==x end)
			if i then remove(vertices, i)	end
		end)
	return v
end

-- create a new spike program at the specified vertex
-- this creates a new object with an update(dt) function
-- the update function returns false once the growth is complete
-- the internal logic is a simple state machine
new_spike = function(the_mesh,the_vertex)
	local states = {
		move = 1,
		inset = 2,
		done = 3
	}

	local SPEED = 4	
	local SEG_LENGTH = .1
	local NUM_SEGS = 5
	local SHRINK = .9
		
	local obj = {
		m=the_mesh,
		v=the_vertex,
		
		n=the_vertex.n,	
		seg = 1,
		distance = 0,
		cap = nil,
		
		state=states.move,
		next_state=nil
		}	
	
	local actions = {}	
	actions[states.move] = function(self,dt)
		local dist = SPEED*dt
		self.v.p = self.v.p + self.n*dist
		if (self.cap) then
			local outer = capov(self.cap)
			local center = vec3(0,0,0)
			for i,ov in ipairs(outer) do
				ov.p = ov.p + self.n*dist
				center = center + ov.p
			end
			center = center/#outer			
			for i,ov in ipairs(outer) do
				local cr = distance(ov.p,center)
				local d = normalise(ov.p-center)
				ov.p = center + d*cr*SHRINK 
			end
			flattenvl(self.m,outer,self.v.p,self.n)
		end			
		self.distance = self.distance + dist			
		if (self.distance > SEG_LENGTH) then
			self.seg = self.seg + 1
			if (self.seg <= NUM_SEGS) then
				self.state = states.inset
			else
				self.state = states.done
			end
		end		
	end

	actions[states.inset] = function(self,dt)
		self.cap = inset(self.m,self.v,.99)
		self.state = states.move
		self.distance = 0
	end	
	
	obj.update = function(self,dt)
		if obj.state==states.done then 
			return false
		else 
			actions[obj.state](obj,dt) 
			return true
		end
	end	
	
	return obj
end