--[[
	A more complex extrusion example.
	BP 12/12/2011	
--]]

module(...,package.seeall)

local opp = {} -- original vertex positions (polar)

local spike, new_spike, m, vertices, v, next_vert
local sf, apply_sf, wave, awave -- superformula function 

function setup()	
	m = icosahedron()
	m:subdivide(3)
	vertices = vertexlist(m)
	for i,v in ipairs(vertices) do
		local r,th,ph = polar(v.p.x,v.p.y,v.p.z)
		ph = ph - pi/2 -- fix the range of ph 
		opp[i] = {r,th,ph}
	end
	m:sync()
	fgu:add(meshnode(m))
		
	apply_sf(
		{a=1,b=1,m=7,n1=1.1,n2=1.1,n3=1.6},
		{a=1,b=1,m=0.48,n1=0.2,n2=1.7,n3=1.7})
	
	spike = nil
end

function update(dt)
	if spike~=nil then
		local more = spike:update(dt)
		if (not more) then 
			spike = nil
		end
	elseif #vertices>0 then
		next_vert()
		spike = new_spike(m,v)
	end
end -- empty

-- get the next random vertex from vertices, 
-- and remove its immediate neighbours
next_vert = function()
	if #vertices==0 then return nil end 
	v= choose(vertices)
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
		
		SPEED = 4,
		PULL_DIST = 0.1,
		TOTAL_SEGS = 7,
		RADII = {.8,.5,.6,.7,.8,.7,.6},
		num_segs = 1,
				
		time=0,
		state=states.waiting,
		next_state=states.pull,
		state_change=.01}	
	
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
		local q = quat(self.axis_of_rotation,.1)
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

apply_sf = function(s1,s2)
	for i,v in ipairs(vertices) do		
		local r,th,ph = opp[i][1],opp[i][2],opp[i][3]		
		v.p = sf(s1,s2,-th,ph) -- negating theta helps..
		v.c = vec3(awave(1.2,ph),awave(1,th),awave(1,0))
	end
end

local function sf_r(p,ang)
	local term = p.m*ang/4	
	return pow(
		pow(abs((1/p.a) * cos(term)),p.n2) + 
		pow(abs((1/p.b) * sin(term)),p.n3),
		-1/p.n1)		
end

-- params: shape 1, shape 2, theta, phi
sf = function(s1,s2,th,ph) 
	local cosphi = cos(ph)
	local r1 = sf_r(s1,th)
	local r2 = sf_r(s2,ph)
	local x = r1*cos(th)*r2*cosphi
	local y = r1*sin(th)*r2*cosphi	
	local z = r2*sin(ph)
	return vec3(x,y,z)
end

wave = function(p,o)
	return sin(p*fgu.t+o)
end
awave = function(p,o)
	return .5+.5*sin(p*fgu.t+o)
end