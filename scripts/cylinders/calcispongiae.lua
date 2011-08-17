
module(...,package.seeall)

require "table"

-- expansion time
n = 1.5
-- rotation angle
delta = math.pi / 15
-- step length
d = 10

local m = {}
local array = {}
local node = {}
function setup()
	-- axiom
	array[1] = new_a(.2,0.,1.)

  	print('Axiom = ' .. arrayToString(array))

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

	--Do the expansions
	if t < n then
		tmp = {}
		k = 0

		for i,v in ipairs(array) do
       	    for j,v2 in ipairs(v:expand()) do
				tmp[j + k] = v2
			end
			k = k + table.getn(v:expand())
		end
	end

	array = tmp

	-- Interperate the result
	donatello = fg.turtle()

	donatello:pitch(math.pi * -.5)

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

	donatello:pushState()
	for i,v in ipairs(array) do
		v:interp(donatello)
	end

	m = donatello:getMesh()
	donatello:popState()
	node:setMesh(m)

	endC = donatello:getEndCap(0)
	local vt = {}
	local mm = m:selectAllVertices()
	for v in mm.all do
		table.insert(vt,v)
	end
	--print(#vt)
	-- vt[endC+1]:setPos(100,0,0)
	fg.extrude(m,vt[endC+1],1,vt[endC+1]:getN(),-.1,-.1)
	fg.extrude(m,vt[endC+1],1,vt[endC+1]:getN(),-.2,-.5)
	fg.extrude(m,vt[endC+1],1,vt[endC+1]:getN(),-.2,.2)
	fg.extrude(m,vt[endC+1],1,vt[endC+1]:getN(),-.2,.2)
	--print(v1)
	--v1:setPos(100,0,0)
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

function new_a(size,initAge,growTime)
	local obj = new_timedSymbol(initAge,growTime)

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
		return {self}
	end

	obj.toString = function(self)
		return 'a'
	end

	obj.interp = function(self,donatello)
	    local a = smoothstep(0, self.gt, self.age) / self.gt
	    local b = smoothstep(0, self.gt*.5, self.age) / self.gt

		local l1 = self.l1 * b
		local l2 = self.l2 * b
		local l3 = self.l3 * b
		local l4 = self.l4 * b
		local l5 = self.l5 * b 

		local w1 = self.w1 * a
		local w2 = self.w2 * a
		local w3 = self.w3 * a
		local w4 = self.w4 * a
		local w5 = self.w5 * a

		donatello:setCarrierMode(0)
		donatello:setScale(w1)
		donatello:beginCylinder()

		donatello:move(l1)
		donatello:setScale(w2)
		donatello:addPoint(2)

		donatello:move(l2)
		donatello:setScale(w3)
		donatello:addPoint(2)

		donatello:move(l3)
		donatello:setScale(w4)
		donatello:addPoint(2)

		donatello:move(l4)
		donatello:setScale(w5)
		donatello:addPoint(2)

		donatello:move(l5)
		donatello:setScale(w3)
		donatello:endCylinder(2)
	end

	return obj
end

function new_b(length,width,angle,initAge,growTime)
	obj = new_timedSymbol(initAge,growTime)

	obj.length = length
	obj.width = width
	obj.angle = angle

	obj.toString = function(self)
		return 'b'
	end

	obj.expand = function(self)
		return {self}
	end

	obj.interp = function (self,donatello)
		size = self.size
		angle = self.angle

		donatello:move(size)
		donatello:pitch(angle / 2)
		donatello:addPoint(2)
		donatello:pitch(angle / 2)
	end

	return obj
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
 	local obj = {
		mN = n
	}

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
	local obj = {
		cs = n
	}

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
