
module(...,package.seeall)

-- num expansions
n = 2
-- rotation angle
delta = math.pi / 4
-- step length
d = 10

local m = {}
local array = {}
local node = {}
function setup()
	-- axiom
	--array[1] = new_Gt(0.6,0.6)
	array[1] = new_Gsc(0.,3,1)
	array[2] = new_Gs()
	array[3] = new_f(0.,3)
	array[4] = new_Gsc(0.,5,.9)
	array[5] = new_Ge(15)

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
  	for i=1,n do
		tmp = {}
		k = 0

		for i,v in ipairs(array) do
       	    for j,v2 in ipairs(v:expand()) do
				tmp[j + k] = v2
			end
			k = k + table.getn(v:expand())
		end

		array = tmp

	  	--print("Expansion " .. i .. " = " .. arrayToString(array))
	end

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

function new_F()
	local obj = {
	}

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_F()
	  return retA;
	end

	obj.toString = function(self)
	  return 'F'
	end

	obj.interp = function(self,turtle)
	  donatello:beginCylinder()
	  donatello:move(d)
	  donatello:endCylinder(1)
	end

	return obj
end

function new_f(initAge,growTime)
	local obj = new_timedSymbol(initAge,growTime) 

	obj.expand = function(self)
	  return {self}
	end

	obj.toString = function(self)
	  return 'f(' .. self.age .. ')'
	end

	obj.interp = function(self,donatello)
	  a = smoothstep(0, self.gt, self.age)

	  donatello:move(d * a)
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


function new_plus()
	local obj = {
	}

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_plus()
	  return retA;
	end

	obj.toString = function(self)
	  return '+'
	end

	obj.interp = function(self,donatello)
	  donatello:yaw(-delta)
	end

	return obj
end

function new_minus()
	local obj = {
	}

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_minus()
	  return retA;
	end

	obj.toString = function(self)
	  return '-'
	end

	obj.interp = function(self,donatello)
	  donatello:yaw(delta)
	end

	return obj
end

function new_ampersand()
	local obj = {
	}

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_ampersand()
	  return retA;
	end

	obj.toString = function(self)
	  return '&'
	end

	obj.interp = function(self,donatello)
	  donatello:pitch(-delta)
	end

	return obj
end

function new_hat()
	local obj = {
	}

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_hat()
	  return retA;
	end

	obj.toString = function(self)
	  return '^'
	end

	obj.interp = function(self,donatello)
	  donatello:pitch(delta)
	end

	return obj
end

function new_backslash()
	local obj = {
	}

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_backslash()
	  return retA;
	end

	obj.toString = function(self)
	  return '\\'
	end

	obj.interp = function(self,donatello)
	  donatello:roll(-delta)
	end

	return obj
end

function new_forwardslash()
	local obj = {
	}

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_forwardslash()
	  return retA;
	end

	obj.toString = function(self)
	  return '/'
	end

	obj.interp = function(self,donatello)
	  donatello:roll(delta)
	end

	return obj
end

function new_pipe()
	local obj = {
	}

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_pipe()
	  return retA;
	end

	obj.toString = function(self)
	  return '|'
	end

	obj.interp = function(self,donatello)
	  donatello:yaw(math.pi)
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

--function new_A()
