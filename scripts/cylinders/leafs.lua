module(...,package.seeall)

local array = {}
local node = {}
local cs = {}
local m = {}
function setup()
	local b = new_branch(12.8,60.,.12,5.)

	donatello = fg.turtle()

	donatello:pitch(math.pi*.41)
	donatello:roll(math.pi*.41)
	donatello:beginCylinder()
	donatello:move(0.1)
	b:interp(donatello)

	--donatello:move(.1)
	--donatello:endCylinder(2)


	for i = 1, donatello:getNumGC(), 1 do
		m[i] = donatello:getMesh(i-1)
		node[i] = fg.meshnode(m[i])
		fgu:add(node[i])
	end
end

local t = 0
function update(dt)
end

function new_branch(numleafs,length,meander,dropoff)
	local obj = {
		n = numleafs,
		l = length,
		m = meander,
		d = dropoff
		}

	obj.interp = function(self,donatello)
		local distance = 0
		local rateCur = 0
		local rateCur2 = 0
		local stepLength = self.l / 20
		local pos = {}
		local sinceL = 0
		local leafDist = self.l / self.n

		donatello:addPoint(2)
		donatello:move(stepLength)
		distance = distance + stepLength

		while (distance < self.l) do
			donatello:yaw(rateCur/2)
			donatello:pitch(rateCur2/2)
			donatello:addPoint()
		
			-- add a pair of leafs
			if sinceL > leafDist then
				local ll = 6. * (self.l - distance / (self.d * 1.15)) / self.l
				local lw = 1.25 * (self.l - distance / (self.d * 1.15)) / self.l
				local bl = 2.5 * (self.l - distance / (self.d * 1.15)) / self.l
				local sc = 0.2 * (self.l - distance / (self.d * 1.3)) / self.l

				local ll1 = randomN( ll, ll *.05 )
				local lw1 = randomN( lw, lw *.05 )
				local l = new_leaf(lw1,ll1, 0.3, 0.1,0.)

				donatello:pushState()
				donatello:roll( -math.pi*.5 )
				donatello:pitch( randomN(math.pi*.25, math.pi*.25*.15) )
				donatello:roll( randomN(math.pi*.5, math.pi*.5*.15) )
				donatello:setScale(sc)
				donatello:beginCylinder()

				donatello:move(bl)
				l:interp(donatello)
				donatello:popState()

				local ll2 = randomN( ll, ll *.05 )
				local lw2 = randomN( lw, lw *.05 )
				local l2 = new_leaf(lw2,ll2, randomN(0.4,0.4*.05), randomN(0.1,0.1*0.05),0.)

				donatello:pushState()
				donatello:roll( -math.pi*.5 )
				donatello:roll( randomN(math.pi, math.pi*0.1) )
				donatello:pitch( randomN(math.pi*.25,math.pi*.25*.15) )
				donatello:roll( randomN(-math.pi*.5,math.pi*.05*.15) )
				donatello:setScale(sc)
				donatello:beginCylinder()

				donatello:move(bl)
				l2:interp(donatello)
				donatello:popState()

				sinceL = 0.

				leafDist = self.l / self.n * (1 - .9*(distance/self.d) / self.l)
			end

			pos = donatello:getPosition()
			rateCur = rateCur + self.m * fracSum(pos.x, pos.y, pos.z, 2, 1)
			rateCur2 = rateCur2 + self.m * fracSum(pos.y, pos.z, pos.x, 1, 1)
			donatello:yaw(rateCur/2)
			donatello:pitch(rateCur2/2)
			donatello:move(stepLength)
			distance = distance + stepLength
			sinceL = sinceL + stepLength
		end
		donatello:yaw(rateCur/2)
		donatello:setScale(1. - 0.7 / self.d)
		donatello:addPoint(2)

		donatello:move(stepLength / 5)
		local ll = 6. * (self.l - distance / 1.3) / self.l
		local lw = 1.25 * (self.l - distance / 1.3) / self.l
		local bl = 2.5 * (self.l - distance / 1.3) / self.l
		local l = new_leaf( .2, .8, 0.4, 0.1, 0.)
		l:interp(donatello)

	end

	return obj
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

		bert:setFrame(vec3(self.w,self.f,-self.b),vec3(0,1,0),vec3(0,0,1))
		bert:setStiffness(0.,0.)
		bert:beginCrossSection()

		bert:setStiffness(0.3,0.3)
		bert:setFrame(vec3(0,self.t,0),vec3(-1,0,0),vec3(0,0,1))
		bert:addPoint()

		bert:setStiffness(0.1,0.1)
		bert:setFrame(vec3(-self.w,self.f,-self.b),vec3(0,-1,0),vec3(0,0,1))
		bert:addPoint()

		bert:setStiffness(0.3,0.3)
		bert:setFrame(vec3(0,-self.t,0),vec3(1,0,0),vec3(0,0,1))
		local ret = bert:endCrossSection()

		return ret
	end

	return obj
end
