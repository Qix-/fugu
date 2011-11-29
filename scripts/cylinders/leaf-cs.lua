module(...,package.seeall)

local array = {}
local node = {}
local cs = {}
local m = {}
function setup()
	local l = new_leaf(1.2, 4., .5, .2, 0.)

	donatello = fg.turtle()

	donatello:setScale(.2)
	donatello:setCarrierMode(0)
	donatello:beginCylinder()
	donatello:move(3.)

	l:interp(donatello)
	m = donatello:getMesh()
	node = fg.meshnode(m)
	fgu:add(node)
end

local t = 0
function update(dt)
end

function new_leaf(width, length, fold, arch, back)
	local obj = {}

	obj.w = width
	obj.l = length
	obj.cs = {}
	obj.n = 3
	obj.f = fold
	obj.b = back
	obj.a = arch
	
--	local t = 0.
--	local n = 4
--	for i = 1, n, 1 do
--		t = t + 1 / n
--		obj.cs[i] = new_leaf_cs(
--	end
	--first seg
	obj.cs[1] = new_leaf_cs(0.2, 0.1, 0., 0)

	obj.cs[2] = new_leaf_cs(0.3, 0.1, .3*obj.f, 0.2*obj.b)
	obj.cs[3] = new_leaf_cs(0.8, 0.1, .8*obj.f, 0.5*obj.b)

	--second seg
	obj.cs[4] = new_leaf_cs(1, 0.1, obj.f, 0.7*obj.b)

	obj.cs[5] = new_leaf_cs(1.1, 0.1, obj.f, 1*obj.b)
	obj.cs[6] = new_leaf_cs(1.1, 0.1, obj.f, 0.8*obj.b)

	--third seg
	obj.cs[7] = new_leaf_cs(0.9, 0.1, .9*obj.f, 0.4*obj.b)

	obj.cs[8] = new_leaf_cs(0.7, 0.1, .7*obj.f, 0.1*obj.b)
	obj.cs[9] = new_leaf_cs(0.3, 0.1, .3*obj.f, 0)

	obj.cs[10] = new_leaf_cs(0.1, 0.1, 0, 0)

	obj.expand = function(self)
	end

	obj.interp = function(self,donatello)
		donatello:pushState()
		local csi = {}
		-- Create the cs
		for i,v in ipairs(self.cs) do
			csi[i] = v:build(donatello)
		end
		donatello:popState()

		-- Draw the leaf
		donatello:setScale(self.w)
		donatello:setCrossSection(csi[1])
		donatello:addPoint()

		local t = 1 / self.n 
		for i = 1, self.n, 1 do
			donatello:pitch(-self.a / 2)
			donatello:move(self.l*.2 / self.n)
			donatello:setCrossSection(csi[1+i])
			donatello:pitch(-self.a / 2)
			donatello:addPoint(1)
			t = t + 1 / self.n
		end

		local t = 1 / self.n
		for i = 1, self.n, 1 do
			donatello:pitch(self.a / 2)
			donatello:move(self.l*.4 / self.n)
			donatello:setCrossSection(csi[self.n+1+i])
			donatello:pitch(self.a / 2)
			donatello:addPoint(1)
			t = t + 1 / self.n
		end

		local t = 1 / self.n
		donatello:setScale(self.w)
		for i = 1, self.n-1, 1 do
			donatello:pitch(self.a / 2)
			donatello:move(self.l*.4 / self.n)
			donatello:setCrossSection(csi[2*self.n+1+i])
			donatello:pitch(self.a / 2)
			donatello:addPoint(1)
			t = t + 1 / self.n
		end

		donatello:move(self.l*.1 / self.n)
		donatello:setCrossSection(csi[3*self.n+1])
		donatello:setScale(self.w * .1)

		donatello:endCylinder(1)
	end

	return obj
end

function new_leaf_cs(width,thickness,fold,back)
	local obj = {
		w = width,
		t = thickness,
		f = fold,
		b = back
	}

	obj.init = function(self)
	end

	obj.build = function(self,bert)
		local up, h

		bert:setFrame(vec3(self.w,self.f,-.3*self.b),vec3(0,1,0),vec3(0,0,1))
		bert:setStiffness(0.,0.)
		bert:beginCrossSection()

--		bert:setFrame(vec3(.6*self.w,0.5*self.t,-1*self.b),vec3(-1,0,0),vec3(0,0,1))
--		bert:setStiffness(0.3,0.3)
--		bert:addPoint()

		bert:setStiffness(0.3,0.3)
		bert:setFrame(vec3(0,self.t,0.),vec3(-1,0,0),vec3(0,0,1))
		bert:addPoint()

--		bert:setFrame(vec3(-.6*self.w,.5*self.t,-1.*self.b),vec3(-1,0,0),vec3(0,0,1))
--		bert:setStiffness(0.3,0.3)
--		bert:addPoint()

		bert:setStiffness(0.0,0.0)
		bert:setFrame(vec3(-self.w,self.f,0.),vec3(0,-1,0),vec3(0,0,1))
		bert:addPoint()

--		bert:setFrame(vec3(-.6*self.w,-.5*self.t,0),vec3(-1,0,0),vec3(0,0,1))
--		bert:setStiffness(0.3,0.3)
--		bert:addPoint()

		bert:setStiffness(0.3,0.3)
		bert:setFrame(vec3(0,-self.t,0),vec3(1,0,0),vec3(0,0,1))
		--bert:addPoint()

--		bert:setFrame(vec3(.6*self.w,-.5*self.t,0),vec3(-1,0,0),vec3(0,0,1))
--		bert:setStiffness(0.3,0.3)
		local ret = bert:endCrossSection()

		return ret
	end

	return obj
end
