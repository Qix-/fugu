--[[
	A more complex extrusion example.
	BP 12/12/2011	
--]]

module(...,package.seeall)

local make_a_thing
local spike, new_spike, n, m, vertices, v, next_vert

-- global parameters
curviness = 0.4
speed = 4
num_segments = 4
width = 0.8
taper = .2
segment_length = 0.1

function setup()	
	add_slider {var="curviness", value=curviness, low=-1, high=1}
	add_slider {var="speed", value=speed, low=0.1, high=10}
	add_slider {var="num_segments", value=num_segments, low=2, high=10}
	add_slider {var="width", value=width, low=0.01, high=2}
	add_slider {var="segment_length", value=segment_length, low=0.01, high=1}
	add_slider {var="taper", value=taper, low=-1, high=1}
	
	make_a_thing()
	n = meshnode(m)
	fgu:add(n)
end

function update(dt) 
	make_a_thing()
	n:set_mesh(m)
end

make_a_thing = function() 
	m = dodecahedron()
	m:smooth_subdivide(1)
	vertices = vertexlist(m)
	spike = nil
	local dt = 0.08
	while #vertices>0 do
		if spike~=nil then
			local more = spike:update(dt)
			if (not more) then 
				spike = nil
			end			
		else
			next_vert()
			spike = new_spike(m,v)
		end
		m:sync()
	end
end

-- get the next random vertex from vertices, 
-- and remove its immediate neighbours
next_vert = function()
	if #vertices==0 then return nil end 
	v = vertices[1] -- choose(vertices)
	local neighbours = loopv(v)
	insert(neighbours,v)
	each(neighbours, 
		function(u)
			i,_ = find(vertices, function(x) return u==x end)
			if i then remove(vertices, i)	end
		end)
end

-- create a new spike program at the specified vertex
-- this creates a new object with an update(dt) function
-- the update function returns false once the growth is complete
-- the internal logic is a simple state machine
new_spike = function(the_mesh,the_vertex)
	local states = {
		waiting = 1,
		pull = 2,
		inset = 3,
		insetagain = 4,
		done = 5
	}

	local obj = {
		m=the_mesh,
		v=the_vertex,
		
		normal=the_vertex.n,
		axis_of_rotation=cross(the_vertex.n,vec3(0,1,0)),			
		
		SPEED = speed,
		PULL_DIST = segment_length,
		TOTAL_SEGS = num_segments,
		RADII = {}, -- // .8,.5,.6,.7,.8,.7,.6},
		num_segs = 1,
				
		time=0,
		state=states.waiting,
		next_state=states.pull,
		state_change=.01}	
		
	for i=1,obj.TOTAL_SEGS do
		obj.RADII[#obj.RADII+1] = pow(width,1+i*taper)
	end
	
	local actions = {}
	actions[states.waiting] = function(self,dt)
		if (self.state_change <= self.time) then
			self.state = self.next_state
		end
	end
	
	actions[states.inset] = function(self,dt)
		inset(self.m,self.v,.8)
		self.state = states.waiting
		self.next_state = states.pull
		self.state_change = self.time + 1
	end
	
	actions[states.pull] = function(self,dt)
		if self.pull_dist==nil then
			self.pull_dist = 0
		end	
		local dist = self.SPEED*dt
		local q = quat(self.axis_of_rotation,curviness)
		local dir = q*self.v.n
		
		self.v.p = self.v.p + dir*dist
		if (self.cap) then
			-- adjust the outer loop on the cap
			-- so it grows in the normal, 
			-- reaches the right scale, 
			-- becomes circular				
			-- and then rotates to orient in the growth dir
			local t = self.pull_dist/self.PULL_DIST -- amount done
			local outer = capov(self.cap)
			local center = vec3(0,0,0)
			for i,ov in ipairs(outer) do
				-- move in growing direction
				ov.p = ov.p + dir*dist
				center = center + ov.p
			end
			center = center/#outer
			local er = self.cap_avg_radius * self.RADII[self.num_segs]
			for i,ov in ipairs(outer) do
				-- make a bit more circular
				-- current, start, end radii
				local cr = distance(ov.p,center)
				local sr = self.cap_radii[i]					
				local tr = max(1,self.SPEED*dt + (cr-sr)/(er-sr))
				local d = normalise(ov.p-center)
				ov.p = center + d*lerp(sr,er,tr)
			end
			-- finally flatten in the growth dir
			flattenvl(self.m,outer,self.v.p,dir)
		end			
		self.pull_dist = self.pull_dist + dist			
		if (self.pull_dist > self.PULL_DIST) then
			self.num_segs = self.num_segs + 1
			if (self.num_segs < self.TOTAL_SEGS) then
				self.state = states.insetagain
			else
				self.state = states.done
			end
		end		
	end

	actions[states.insetagain] = function(self,dt)
		-- inset a tiny bit and store the cap for subsequent pulls								
		self.cap = inset(self.m,self.v,.99)
		-- compute the avg radius of this cap so we can circulise it
		-- also store the current radius of each outer vert
		self.cap_radii = {}
		local outer = capov(self.cap)
		local center = vec3(0,0,0)			
		for _,ov in ipairs(outer) do
			center = center + ov.p
		end
		center = center/#outer
		local avg_radius = 0
		for _,ov in ipairs(outer) do				
			local r = distance(ov.p,center)
			self.cap_radii[#self.cap_radii+1] = r
			avg_radius = avg_radius + r
		end
		avg_radius = avg_radius/#outer
		self.cap_avg_radius = avg_radius		
		self.state = states.pull
		self.pull_dist = nil
	end	
	
	obj.update = function(self,dt) 
		self.time = self.time + dt
		if obj.state==states.done or obj.state==nil then 
			return false
		else 
			actions[obj.state](obj,dt) 
			return true
		end
	end	
	
	return obj
end