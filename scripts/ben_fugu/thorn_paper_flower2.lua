module(...,package.seeall)

local m, spike, new_spike, next_vert

function setup()
	m = icosahedron()
	m:smooth_subdivide(4) -- 3
	local n = meshnode(m)
	fgu:add(n)
	
	vertices = {}
	local all_vertices = vertexlist(m)
	while #all_vertices>0 do 
		vertices[#vertices+1] = next_vert(all_vertices)
	end
	spike = nil
end
	
	--[[
	spikes = {}
	for i=1,6 do
		spikes[i] = new_spike(m,vertices[i])
	end
	--[ [
	each(vertices, function(v)
		spikes[#spikes+1] = new_spike(m,v)
	end)
	
end
--]]

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
	local v = choose(vertices)
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
	
	local dy = (the_vertex.p.y+1.2)/2.4

	local obj = {
		m=the_mesh,
		v=the_vertex,
		
		normal=the_vertex.n,
		axis_of_rotation=cross(the_vertex.n,vec3(0,1,0)),			
		seg = 1,
		
		-- SPEED = (1.2+(2-the_vertex.p.y)) * 3, -- 4,
		SPEED = sqr((1.5-dy) * 3), -- 4,
		
		SEG_LENGTH = sqr(1-dy)*0.5,
		NUM_SEGS = 10, -- 7,
		--RADII = {.8,.5,.6,.7,.8,.7,.6, 1.2, 3, .5},
		RADII = {.9,.7,.8,.9,.8,.8,.7, 1.2, 3, .8},
		ROTATION_RAD = .1 + (1-dy)*.05,
		
		--time=0,
		state=states.move,
		next_state=nil,
		--state_change=.01
		}	
	
	-- don't extrude things pointing directly up or down
	if (length(obj.axis_of_rotation)<.01) then return nil end
	
	local actions = {}	
	actions[states.move] = function(self,dt)
		if self.distance==nil then
			self.distance = 0
		end	
		local dist = self.SPEED*dt
		local q = quat(self.axis_of_rotation,self.ROTATION_RAD)
		local dir = q*self.v.n
		
		self.v.p = self.v.p + dir*dist
		if (self.cap) then
			-- adjust the outer loop on the cap
			-- so it grows in the normal, 
			-- reaches the right scale, 
			-- becomes circular				
			-- and then rotates to orient in the growth dir
			local t = self.distance/self.SEG_LENGTH -- amount done
			local outer = capov(self.cap)
			local center = vec3(0,0,0)
			for i,ov in ipairs(outer) do
				-- move in growing direction
				ov.p = ov.p + dir*dist
				center = center + ov.p
			end
			center = center/#outer
			local er = self.cap_avg_radius * self.RADII[self.seg]
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
		self.distance = self.distance + dist			
		if (self.distance > self.SEG_LENGTH) then
			self.seg = self.seg + 1
			if (self.seg < self.NUM_SEGS) then
				self.state = states.inset
			else
				self.state = states.done
			end
		end		
	end

	actions[states.inset] = function(self,dt)
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
		self.state = states.move
		self.distance = nil
	end	
	
	obj.update = function(self,dt) 
		--self.time = self.time + dt
		if obj.state==states.done or obj.state==nil then 
			return false
		else 
			actions[obj.state](obj,dt) 
			return true
		end
	end	
	
	return obj
end