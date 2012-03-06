
module(...,package.seeall)

local m, spike, new_spike, next_vert

function setup()
	m = icosahedron()
	m:smooth_subdivide(2)
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
	
	local SHRINK = 0.8
	local SEG_LENGTH = .1
	local NUM_SEGS = 10
		
	local NUM_SUB_STEPS = 6
	local SUBSEG_RADII = {.9, 1.1, 1.15, 1, 1, .9}
	local SUBSEG_DIST = {.3, .3, .3, .1, -.5, 2} 
				
	local obj = {
		m=the_mesh,
		v=the_vertex,
		
		axis_of_rotation=cross(the_vertex.n,vec3(0,1,0)),
		
		seg = 1,
		cap = loopp(the_vertex),
		
		ROTATION_RAD = .2,
		}
		
	-- don't extrude things pointing directly up or down
	if (length(obj.axis_of_rotation)<.01) then return nil end
	
	obj.cap_radii = {}
	local outer = capov(obj.cap)
	local center = vec3(0,0,0)			
	for _,ov in ipairs(outer) do
		center = center + ov.p
	end
	center = center/#outer
	local avg_radius = 0
	for _,ov in ipairs(outer) do				
		local r = distance(ov.p,center)
		obj.cap_radii[#obj.cap_radii+1] = r
		avg_radius = avg_radius + r
	end
	avg_radius = avg_radius/#outer
	obj.cap_avg_radius = avg_radius	
		
	obj.extrude = function(self, dir, dist, radii)
		local dist = dist*SEG_LENGTH
		self.v.p = self.v.p + dir*dist
		local outer = capov(self.cap)
		local center = vec3(0,0,0)
		for i,ov in ipairs(outer) do
			ov.p = ov.p + dir*dist
			center = center + ov.p
		end
		center = center/#outer
		local er = self.cap_avg_radius * radii
		for i,ov in ipairs(outer) do
			-- local cr = distance(ov.p,center)
			-- local sr = self.cap_radii[i]					
			--local tr = max(1,self.SPEED*dt + (cr-sr)/(er-sr))
			local d = normalise(ov.p-center)
			ov.p = center + d*er -- lerp(sr,er,tr)
			--ov.p = center + d*cr*radii 
		end				
		flattenvl(self.m,outer,self.v.p,dir)
		
		self.cap = inset(self.m,self.v,.99)
		
		local avg_radius = 0
		for _,ov in ipairs(outer) do				
			local r = distance(ov.p,center)
			self.cap_radii[#self.cap_radii+1] = r
			avg_radius = avg_radius + r
		end
		avg_radius = avg_radius/#outer
		self.cap_avg_radius = avg_radius	
	end
	
	obj.update = function(self,dt)
		local last_scale = 1		
		local q = quat(self.axis_of_rotation,self.ROTATION_RAD)
		for i=1,NUM_SUB_STEPS do
			local dir = q*self.v.n
			print(dir)
			self:extrude(dir,SUBSEG_DIST[i],SUBSEG_RADII[i]/last_scale)
			last_scale = SUBSEG_RADII[i]
			self.m:sync()
			--self.v:calculate_normal()
		end
		self.seg = self.seg + 1
		if (self.seg <= NUM_SEGS) then
			return true
		else
			return false
		end		
	end
	return obj
end