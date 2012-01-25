--[[
	test
--]]

module(...,package.seeall)

someGlobalVar = 1

local smoothGrowth -- object which performs the smooth growth
local newSmoothGrowth -- constructor for object
local nextvert 
local n, m
local vertices, vertex

local opp = {} -- original vertex positions (polar)
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
	for _,v in ipairs(vertices) do
		v:set_uv(0,0)
	end	
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
		i,_ = find(vertices, function(x) return u==x end)
		if i then table.remove(vertices, i)	end
	end
	
	return v	
end

newSmoothGrowth = function(m,v)
	local obj = {
		m=m,
		v=v,		
		INSET = .8, -- scale on first inset 
		SPEED = .7,
		CIRC_SPEED = .5, -- speed the cap becomes circular
		FL_SPEED = .1, -- speed the cap flattens
		PULLDIST = .3, -- distance to pull the leading vertex
		NUMSEGS = 4,
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
	local vloop = loopv(v)
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
			if (self.stateChange <= self.time) then
				self.state = self.nextState
			end
		elseif (self.state=="inset") then
			inset(self.m,self.v,self.INSET)
			self.state = "waiting"
			self.nextState = "pull"
			self.stateChange = self.time + 1
		elseif (self.state=="pull") then 
			if self.pullDist==nil then
				self.pullDist = 0
			end	
			local dist = self.SPEED*dt
			self.v.p = self.v.p + self.v.n*dist*self.pullDir
			
			-- adjust the outer loop on the cap
			-- so it grows in the normal, 
			-- reaches the right scale, 
			-- and becomes circular
			if (self.cap) then			
				local t = self.pullDist/self.PULLDIST -- amount done
				self.v:set_colour(lerp(startColour,targetColour,t))
				self.v:set_uv(ds,0)
				local outer = capov(self.cap)

				-- move the cap in the primary growth axis
				-- and calculate the current cap center position
				local center = vec3(0,0,0)
				for i,ov in ipairs(outer) do									
					ov.p = ov.p + self.v.n*dist*self.pullDir
					local flattendist = ds*dot(ov.p-self.v.p,self.v.n)
					ov.p = ov.p - self.v.n*flattendist

					-- (dist + math.max(self.FL_SPEED*dt*flattendir,flattendist))
					-- ov.p = ov.p + self.v.n*dist
					ov:setColour(self.v.c)
					ov:setUV(ds,0)
					center = center + ov.p
				end
				center = center/#outer
		
				-- adjust the cap verts to make 
				-- them more circular				
				for i,ov in ipairs(outer) do
					-- current radius, start radius
					local cr = distance(ov.p,center)
					local sr = self.capRadii[i]
					local t
					if (self.capAvgRadius*someGlobalVar-sr==0) then
						t = 1
					else
						t = (cr - sr) / (self.capAvgRadius*someGlobalVar - sr)
					end
					local tr = max(1, t + self.CIRC_SPEED*dt) 
					local d = normalise(ov.p-center)
					ov.p = center + d*lerp(sr,self.capAvgRadius*someGlobalVar,tr)
				end
			end			

			self.pullDist = self.pullDist + dist			
			if (self.pullDist > self.PULLDIST) then
				self.segs = self.segs + 1
				if (self.segs < self.NUMSEGS) then
					if (self.pullDir > 0) then					
						self.state = "insetagain"
					else
						self.state = "inwardsinset"
					end						
				else
					if (self.pullDir > 0) then
						self.state = "inwardsinset"
						self.segs = 1
					else
						self.state = "done"
					end
				end
			end		
		elseif (self.state=="insetagain") then
			-- inset a tiny bit and store the cap for subsequent pulls
			local insetVal = .99
			if self.cap==nil then			
				insetVal = self.INSET
			end
			obj:insetCap(insetVal,startColour,{ds,0})
			self.state = "pull"
			self.pullDist = nil --reset
			self.pullDir = 1			
		elseif (self.state=="inwardsinset") then 
			-- do an "inny"!
			obj:insetCap(.6,startColour,{1,0})
			self.state = "pull"
			self.pullDist = nil --reset
			self.pullDir = -1			
		elseif (self.state=="done") then 
			return false -- finished!
		end			
		 
		return true
	end
	
	obj.insetCap = function(self, amount, col, uv)
		self.cap = inset(self.m,self.v,amount)
		-- compute the avg radius of this cap so we can circulise it
		-- also store the current radius of each outer vert
		self.capRadii = {}
		local outer = capov(self.cap)
		local center = vec3(0,0,0)
		for i,ov in ipairs(outer) do
			center = center + ov.p
			ov:set_colour(col)
			ov:set_uv(uv[1],uv[2])			
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