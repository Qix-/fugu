
module(...,package.seeall)

require "table"

-- expansion time
n = 17.9
-- rotation angle
delta = math.pi / 15
-- step length
d = 10

local m = {}
local array = {}
local node = {}
local cs = {}
function setup()
	-- axiom
--	array[1] = new_Gs()
--	array[2] = new_c(2.,1.,6,0,4)
--	array[3] = new_Ge(2)
	array[1] = new_Gcs(1)
	array[2] = new_Gsc(0,4,1.5)
	array[3] = new_Gs()
	array[4] = new_Gcs(0)
	array[5] = new_a(2.,1.,6,.1,0,4)
	array[6] = new_Ge(5)

  	print('Axiom = ' .. arrayToString(array))

	cs[1] = new_bumpy_cs(5, 1.5, .2, 0, 1)
	cs[1]:init()

	donatello = fg.turtle()

	donatello:pushState()
    donatello:setFrame(vec3(.5,-.5,0.),vec3(0.,1.,0.),vec3(0.,0.,1.))
	donatello:setStiffness(0.9,0.9)

	donatello:beginCrossSection()
	donatello:move(1.)
	donatello:addPoint()
	donatello:yaw(-math.pi*.5)
	donatello:move(1.)
	donatello:yaw(-math.pi*.5)
	donatello:addPoint()
	donatello:move(1.)
	donatello:yaw(-math.pi*.5)
	donatello:endCrossSection()

	donatello:popState()

	m = donatello:getMesh()
	node = fg.meshnode(m)
	fgu:add(node)

end

local t = 0
function update(dt)
	t = t + dt

	--Update symbols
	for i,v in ipairs(array) do
		v:update(dt)
	end

	cs[1]:update(dt)

	--Do the expansions
	if t < n then
		tmp = {}
		k = 1

		for i,v in ipairs(array) do
       	    for j,v2 in ipairs(v:expand()) do
				tmp[k] = v2
				
				k = k + 1
			end
		end
	end

	array = tmp
  	--print('Axiom = ' .. arrayToString(array))

	-- Interperate the result
	donatello = fg.turtle()

	donatello:pitch(math.pi * -.5)

	donatello:pushState()
	cs[1]:build(donatello)
--	cs[2]:build(donatello)
--	cs[3]:build(donatello)
	donatello:popState()
--
--	donatello:setCrossSection(1)
--	donatello:setCarrierMode(1)
--	donatello:setScale(1.5)
--	donatello:beginCylinder()
--	donatello:pitch(math.pi * 0.03)
--	donatello:move(5)
--
--	donatello:setCrossSection(2)
--	donatello:setScale(1.)
--	donatello:pitch(math.pi * 0.03)
--	donatello:addPoint(10)
--	donatello:pitch(math.pi * 0.03)
--
--	donatello:move(8)
--	donatello:pitch(math.pi * 0.03)
--	donatello:setCrossSection(3)
--	donatello:endCylinder(10)

	if t > 0 and t < 27.917 then
		donatello:pushState()
		donatello:setCarrierMode(0)
		for i,v in ipairs(array) do
			v:interp(donatello)
		end

		m = donatello:getMesh()
		donatello:popState()
		node:setMesh(m)

--		i = 0;
--		while (i < donatello:getNumEndCaps()) do
--			endC = donatello:getEndCap(i)
--			local vt = {}
--			local mm = m:selectAllVertices()
--			for v in mm.all do
--				table.insert(vt,v)
--			end
--			fg.extrude(m,vt[endC+1],1,vt[endC+1]:getN(),-.1,-.1)
--			fg.extrude(m,vt[endC+1],1,vt[endC+1]:getN(),-.2,-.5)
--			fg.extrude(m,vt[endC+1],1,vt[endC+1]:getN(),-.2,.2)
--			fg.extrude(m,vt[endC+1],1,vt[endC+1]:getN(),-.2,.2)
--
--			i = i + 1
--		end
	end
end

function arrayToString(arr)
    outS = ''
	for i,v in ipairs(arr) do 
		outS = outS .. arr[i]:toString() 
	end
	return outS
end

function copyArray(a1)
	a2 = {}
    for i,v in pairs(a1) do
		a2[i] = v
	end
	return a2
end

function new_Gs()
	local obj = new_timedSymbol(0,0)

	obj.expand = function(self)
	  return {self}
	end

	obj.toString = function(self)
	  return 'Gs'
	end

	obj.interp = function(self,donatello)
	  donatello:beginCylinder()
	end

	return obj
end

function new_Gc(n)
	local obj = new_timedSymbol(0,1)

	obj.mN = n

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_Gc(self.mN)
	  return retA;
	end

	obj.toString = function(self)
	  return 'Gc(' .. self.mN .. ')'
	end

	obj.interp = function(self,donatello)
	  donatello:addPoint(self.mN)
	end

	return obj
end

function new_Ge(n)
 	local obj = new_timedSymbol(0,0)
	
	obj.mN = n

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_Ge(self.mN)
	  return retA;
	end

	obj.toString = function(self)
	  return 'Ge(' .. self.mN .. ')'
	end

	obj.interp = function(self,donatello)
	  donatello:endCylinder(self.mN)
	end

	return obj
end

function new_Gt(t1,t2)
	local obj = new_timedSymbol(0,0)

	obj.tan1 = t1
	obj.tan2 = t2

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_Gt(self.tan1, self.tan2)
	  return retA
	end

	obj.toString = function(self)
	  return 'Gt(' .. self.tan1 .. ', ' .. self.tan2 .. ')'
	end

	obj.interp = function(self,donatello)
	  donatello:setStiffness(self.tan1,self.tan2)
	end

	return obj
end

function new_Gsc(initAge,growTime,s)
	local obj = new_timedSymbol(initAge,growTime)

	obj.scale = s

	obj.expand = function(self)
	  return {self}
	end

	obj.toString = function(self)
	  return 'Gsc(' .. self.scale .. ')'
	end

	obj.interp = function(self,donatello)
	  a = smoothstep(0, self.gt, self.age)

	  donatello:setScale(self.scale * a)
	end

	return obj
end

function new_Gcs(n)
	local obj = new_timedSymbol(0, 1)

	obj.cs = n

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_Gcs(self.cs)
	  return retA
	end

	obj.toString = function(self)
	  return 'Gcs(' .. self.cs .. ')'
	end

	obj.interp = function(self,donatello)
	  donatello:setCrossSection(self.cs)
	end

	return obj
end

function new_timedSymbol(initAge, growTime)
	local obj = {
		age = initAge,
		gt = growTime
	}

	obj.update = function(self, dt)
	  self.age = self.age + dt
	end

	return obj
end

function new_bumpy_cs(nbumps,size,noise,initAge,growTime)
	local obj = new_timedSymbol(initAge,growTime)

	obj.nbumps = nbumps
	obj.size = size
	obj.noise = noise

	obj.t = {}
	obj.s = {}

	obj.init = function(self)
		theta = 0
		thetaInc = math.pi / self.nbumps

		i = 1
		while (i <= self.nbumps * 2) do
			-- non bump
			self.t[i] = randomN(theta, thetaInc * self.noise)
			self.s[i] = randomN(1, self.noise)

			i = i + 1
			theta = theta + thetaInc

			-- bump
			self.t[i] = randomN(theta, thetaInc * self.noise)
			self.s[i] = randomN(self.size, (self.size - 1) * self.noise)

			i = i + 1
			theta = theta + thetaInc
		end
	end

	obj.build = function(self,bert)
		--local a = (self.size - 1) * smoothstep(0, self.gt, self.age)

		-- first point (not a bump)
		y = self.s[1] * math.sin(self.t[1])
		x = self.s[1] * math.cos(self.t[1])

		local dy = math.cos(self.t[1])
		local dx = -math.sin(self.t[1])

		bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))

		bert:beginCrossSection()

		-- add a bump
		y = self.s[2] * math.sin(self.t[2])
		x = self.s[2] * math.cos(self.t[2])

		local dy = math.cos(self.t[2])
		local dx = -math.sin(self.t[2])

		bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))

		i = 3
		while (i <= self.nbumps * 2) do
			bert:addPoint()

			-- add the next non bump
			y = math.sin(self.t[i])
			x = math.cos(self.t[i])

			dy = math.cos(self.t[i])
			dx = -math.sin(self.t[i])

			bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))
			bert:addPoint()

			i = i + 1

			-- add the next bump
			y = self.s[i] * math.sin(self.t[i])
			x = self.s[i] * math.cos(self.t[i])

			dy = math.cos(self.t[i])
			dx = -math.sin(self.t[i])

			bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))

			i = i + 1
		end

		return bert:endCrossSection()
	end

	return obj
end

function new_a(length,width,nsegs,meander,initAge,growTime)
	local obj = new_timedSymbol(initAge,growTime)

	obj.length = length
	obj.width = width
	obj.nsegs = nsegs
	obj.drawnsegs = 0

	obj.toString = function(self)
	  return 'a'
	end

	obj.interp = function(self, donatello)
	end

	obj.expand = function(self)
		local stage = self.age / self.gt * self.nsegs

		if stage > self.drawnsegs then
			self.drawnsegs = self.drawnsegs + 1
			local reta = {}
			reta[1] = new_f(self.length,0,self.gt/2)
			reta[2] = new_hat(0.05, 0, 1)
			reta[3] = new_Gsc(0,self.gt,self.width*(1-stage/(2*self.nsegs)))
			reta[4] = new_Gc(2)
			reta[5] = new_hat(0.05, 0, 1)
			reta[6] = self
			return reta
		end
		if stage > self.nsegs then
			reta = {}
			reta[1] = new_c(self.length*.5,self.width*(1-stage/(2*self.nsegs)),2.7*self.nsegs,0,self.gt)
			return reta
		end

		return {self}
	end

	return obj
end

function new_b(length,width,nsegs,meander,initAge,growTime)
	obj = new_timedSymbol(initAge,growTime)

	obj.length = length
	obj.width = width
	obj.nsegs = nsegs
	obj.drawnsegs = 0
	obj.meander = meander

	obj.toString = function(self)
	  return 'b'
	end

	obj.interp = function(self, donatello)
	end

	obj.expand = function(self)
		local stage = self.age / self.gt * self.nsegs

		if stage > self.drawnsegs then
			if self.drawnsegs < self.nsegs then
				theta = randomN(self.meander*math.cos(-math.pi/3+(1.4*math.pi)*stage/self.nsegs), 0.05)
				self.drawnsegs = self.drawnsegs + 1
				reta = {}
				reta[1] = new_f(self.length,0,self.gt/2)
				reta[2] = new_hat(theta, 0, 1)
				reta[3] = new_Gsc(0,self.gt,self.width)
				reta[4] = new_hat(theta, 0, 1)
				reta[5] = new_Gc(2)
				reta[6] = self
				return reta
			else
				self.drawnsegs = self.drawnsegs + 1
				reta = {}
				reta[1] = new_f(self.length,0,self.gt/2)
				reta[2] = new_hat(-0.05, 0, 1)
				reta[3] = new_Gsc(0,self.gt,self.width)
				reta[4] = new_hat(-0.05, 0, 1)
				reta[5] = new_Gc(2)
				reta[6] = self
				return reta
			end
		end
		if stage > self.nsegs then
			return {new_d(self.width,0,self.gt)}
		end

		return {self}
	end

	return obj
end

function new_c(length,width,nsegs,initAge,growTime)
	obj = new_timedSymbol(initAge,growTime)

	obj.n = n
	obj.length = length
	obj.width = width
	obj.nsegs = nsegs
	obj.theta = 0
	obj.drawnsegs = 0

	obj.toString = function(self)
		return 'c'
	end

	obj.interp = function(self,donatello)
	end

	obj.expand = function(self)
		local stage = self.age / self.gt * self.nsegs

		if stage > self.drawnsegs then
			self.theta = self.theta + randomN(2.39982772,0.01)--math.pi*2 / self.nsegs 
			local a = (1-stage/(2*self.nsegs))
			local b = (1-stage/(25*self.nsegs))

			self.drawnsegs = self.drawnsegs + 1
			self.length = self.length * b 
			reta = {}
			reta[1] = new_push()
			reta[2] = new_backslash(self.theta,0,0.1)
			reta[3] = new_Gs()
			reta[4] = new_b(self.length,self.width*a,.5*self.nsegs,.1*a,0,self.gt)
			--reta[5] = new_Gsc(0,self.gt,0.1);
			reta[5] = new_Ge(2)
			reta[6] = new_pop()
			reta[7] = new_f(self.length,0,self.gt/2)
			reta[8] = new_Gsc(0,self.gt,self.width*(1-stage/(2*self.nsegs)))
			reta[9] = new_Gc(2)
			reta[10] = self
			return reta
		end
		if stage > self.nsegs then
			return {}--new_Ge(2)}
		end

		return{self}
	end

	return obj
end

function new_f(length,initAge,growTime)
	local obj = new_timedSymbol(initAge,growTime) 
	obj.length = length

	obj.expand = function(self)
	  return {self}
	end

	obj.toString = function(self)
	  return 'f(' .. self.age .. ')'
	end

	obj.interp = function(self,donatello)
	  local a = smoothstep(0, self.gt, self.age)

	  donatello:move(self.length * a)
	end

	return obj
end

function new_ampersand(maxAngle, initAge, growTime)
	local obj = new_timedSymbol(initAge, growTime)

	obj.angle = maxAngle

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_ampersand()
	  return retA;
	end

	obj.toString = function(self)
	  return '&'
	end

	obj.interp = function(self,donatello)
	  a = self.angle * smoothstep(0, self.gt, self.age) / self.gt
	  donatello:pitch(-a)
	end

	return obj
end

function new_hat(maxAngle, initAge, growTime)
	local obj = new_timedSymbol(initAge, growTime)

	obj.angle = maxAngle

	obj.expand = function(self)
	  return {self}
	end

	obj.toString = function(self)
	  return '^'
	end

	obj.interp = function(self,donatello)
	  a = self.angle * smoothstep(0, self.gt, self.age) / self.gt
	  donatello:pitch(a)
	end

	return obj
end

function new_backslash(maxAngle, initAge, growTime)
	local obj = new_timedSymbol(initAge, growTime)

	obj.angle = maxAngle

	obj.expand = function(self)
	  return {self}
	end

	obj.toString = function(self)
	  return '\\'
	end

	obj.interp = function(self,donatello)
	  a = self.angle * smoothstep(0, self.gt, self.age) / self.gt
	  donatello:roll(-a)
	end

	return obj
end

function new_forwardslash(maxAngle, initAge, growTime)
	local obj = new_timedSymbol(initAge, growTime)

	obj.angle = maxAngle

	obj.expand = function(self)
	  return {self}
	end

	obj.toString = function(self)
	  return '/'
	end

	obj.interp = function(self,donatello)
	  a = self.angle * smoothstep(0, self.gt, self.age) / self.gt
	  donatello:roll(a)
	end

	return obj
end

function new_push()
	local obj = new_timedSymbol(0, 1)

	obj.expand = function(self)
		return {self}
	end

	obj.toString = function(self)
		return '['
	end

	obj.interp = function(self,donatello)
	  donatello:pushState()
	end

	return obj
end

function new_pop()
	local obj = new_timedSymbol(0, 1)

	obj.expand = function(self)
		return {self}
	end

	obj.toString = function(self)
		return ']'
	end

	obj.interp = function(self,donatello)
	  donatello:popState()
	end

	return obj
end

function new_d(size,initAge,growTime)
	local obj = new_timedSymbol(initAge,growTime)

	obj.drawnSeg = 0
	obj.l1 = size * 5
	obj.l2 = size * 4
	obj.l3 = size * 2.5
	obj.l4 = size * .1
	obj.l5 = size * .5

	obj.w1 = size * 1
	obj.w2 = size * 3
	obj.w3 = size * 1.2
	obj.w4 = size * 1.4
	obj.w5 = size * 1.7

	obj.expand = function(self)
		local stage = self.age / self.gt * 5

		if self.drawnSeg == 0 and stage > 1 then
			self.drawnSeg = self.drawnSeg + 1
			reta = {}
			reta[1] = new_f(self.l1,0,self.gt)
			reta[2] = new_Gsc(0,self.gt,self.w2)
			reta[3] = new_Gc(2)
			reta[4] = self
			return reta
		elseif self.drawnSeg == 1 and stage > 2 then
			self.drawnSeg = self.drawnSeg + 1
			reta = {}
			reta[1] = new_f(self.l2,0,self.gt)
			reta[2] = new_Gsc(0,self.gt,self.w3)
			reta[3] = new_Gc(2)
			reta[4] = self
			return reta
		elseif self.drawnSeg == 2 and stage > 3 then
			self.drawnSeg = self.drawnSeg + 1
			reta = {}
			reta[1] = new_f(self.l3,0,self.gt)
			reta[2] = new_Gsc(0,self.gt,self.w4)
			reta[3] = new_Gc(2)
			reta[4] = self
			return reta
		elseif self.drawnSeg == 3 and stage > 4 then
			self.drawnSeg = self.drawnSeg + 1
			reta = {}
			reta[1] = new_f(self.l4,0,self.gt)
			reta[2] = new_Gsc(0,self.gt,self.w5)
			reta[3] = new_Gc(2)
			reta[4] = self
			return reta
		elseif self.drawnSeg == 4 and stage > 5 then
			self.drawnSeg = self.drawnSeg + 1
			reta = {}
			reta[1] = new_f(self.l5,0,self.gt)
			reta[2] = new_Gsc(0,self.gt,self.w3)
			reta[3] = new_Gc(2)
			return reta
		end

		return{self}
	end

	obj.toString = function(self)
		return 'd'
	end

	obj.interp = function(self,donatello)
--	    local a = smoothstep(0, self.gt, self.age) / self.gt
--	    local b = smoothstep(0, self.gt*.5, self.age) / self.gt
--
--		local l1 = self.l1 * b
--		local l2 = self.l2 * b
--		local l3 = self.l3 * b
--		local l4 = self.l4 * b
--		local l5 = self.l5 * b 
--
--		local w1 = self.w1 * a
--		local w2 = self.w2 * a
--		local w3 = self.w3 * a
--		local w4 = self.w4 * a
--		local w5 = self.w5 * a
--
--		donatello:setCarrierMode(0)
--		donatello:setScale(w1)
--		donatello:beginCylinder()
--
--		donatello:move(l1)
--		donatello:setScale(w2)
--		donatello:addPoint(2)
--
--		donatello:move(l2)
--		donatello:setScale(w3)
--		donatello:addPoint(2)
--
--		donatello:move(l3)
--		donatello:setScale(w4)
--		donatello:addPoint(2)
--
--		donatello:move(l4)
--		donatello:setScale(w5)
--		donatello:addPoint(2)
--
--		donatello:move(l5)
--		donatello:setScale(w3)
--		donatello:endCylinder(2)
	end

	return obj
end
