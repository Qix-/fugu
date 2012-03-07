module(...,package.seeall)

-- expansion time
n = 17.9

local array = {}
local node = {}
local cs = {}
local m = {}
function setup()
	-- axiom
	array[1] = new_Gcs(0)
	array[2] = new_Gs()
	array[3] = new_f(0,0,.5)
	array[4] = new_leaf(2,4)
	array[5] = new_Ge(5)

  	print('Axiom = ' .. arrayToString(array))

	cs[1] = new_leaf_cs()
	cs[1]:init()

	donatello = fg.turtle()
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
	cs[1]:build(donatello)
	donatello:popState()

	donatello:pushState()
	for i,v in ipairs(array) do
		v:interp(donatello)
	end

	m = donatello:getMesh()
	donatello:popState()
	node:setMesh(m)
end

function new_leaf(width, length)
	local obj = new_timedSymbol(0,0)
	obj.w = width
	obj.l = length
	obj.aw = 0.2
	obj.bw = 0.9
	obj.cw = .3
	obj.al = 0.2
	obj.bl = 0.4
	obj.cl = 0.4

	obj.cs = new_leaf_cs()
	obj.cs:init()

	obj.toString = function(self)
	  return 'L'
	end

	obj.expand = function(self)
	  return {new_Gcs(1),
	          new_Gsc(2,1,self.aw*self.w),
			  new_Gc(2),

			  new_f(0,0,self.al*self.l),
			  new_Gsc(2,1,self.w),
			  new_Gc(2),

			  new_f(2,1,self.bl*self.l),
			  new_Gsc(2,1,self.bw*self.w),
			  new_Gc(2),

			  new_f(2,1,self.cl*self.l),
			  new_Gsc(2,1,self.cw*self.w),
			  --new_Gcs(1),
			  new_Gc(5),

			  new_f(2,1,0.1*self.cl*self.l),
			  --new_Gcs(0),
			  new_Gsc(2,1,self.cw*self.w),
			  }
	end

	obj.interp = function(self,donatello)
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

function new_f(initAge,growTime,length)
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

function new_leaf_cs()
	local obj = new_timedSymbol(0,0)

	obj.init = function(self)
	end

	obj.build = function(self,bert)
		bert:setFrame(vec3(1,0,.0),vec3(0,1,0),vec3(0,0,1))
		bert:setStiffness(0.1,0.1)
		bert:beginCrossSection()

		bert:setStiffness(0.3,0.3)
		bert:setFrame(vec3(0,0.1,0),vec3(-1,0,0),vec3(0,0,1))
		bert:addPoint()

		bert:setStiffness(0.1,0.1)
		bert:setFrame(vec3(-1,0,.0),vec3(0,-1,0),vec3(0,0,1))
		bert:addPoint()

		bert:setStiffness(0.3,0.3)
		bert:setFrame(vec3(0,-0.1,0),vec3(1,0,0),vec3(0,0,1))
		return bert:endCrossSection()
	end

	return obj
end
