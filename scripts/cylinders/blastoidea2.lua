module(...,package.seeall)

local m = {}
local node = {}

function setup()

--	-- roots
--	root = new_root(15,.5,.5)
--
--	don = fg.turtle()
--	root:build(don)
--	m[1] = don:getMesh()
--	node[1] = fg.meshnode(m[1])

    -- stalk
	stalk = new_stalk(.3,.5,30)

    -- holdfast
	hold_fast = new_holdfast(5,.5,5,.1)

	-- calyx
	calyx = new_cylax(5,.5,5)

    donatello = fg.turtle()

    donatello:pushState()

	donatello:pitch(math.pi)
	hold_fast:build(donatello)

	donatello:popState()
	stalk:build(donatello)

	calyx:build(donatello)

	node = fg.meshnode(donatello:getMesh())
	fgu:add(node)

end

local t = 0
function update(dt)

end

function new_root(length,width,meander)
	local obj = {
		rootLength  = length,
		rootWidth   = width,
		rootMeander = meander,
		stepLength  = length / 8
	}

	obj.build = function(self,bert)
		-- Create the cylinder

		-- First the turn
		bert:setScale(self.rootWidth)
		bert:setStiffness(0.3,0.3)
		bert:beginCylinder()
		bert:move(self.rootWidth)
		bert:pitch(math.pi * .25)
		bert:move(self.rootWidth)
		bert:setScale(self.rootWidth * .7)
		bert:addPoint(10)
		bert:move(self.rootWidth)
		bert:pitch(math.pi * .25)
		bert:move(self.rootWidth)
		bert:setScale(self.rootWidth * .5)
		
		-- Now the wiggly bit
		distance = 0
		rateCur = 0
		stepLength = self.stepLength
		bert:addPoint(10)
		bert:move(self.stepLength)
		distance = distance + stepLength
--
		while (distance < self.rootLength) do
			if (distance > .6 * self.rootLength) then
			--	bert:setScale(0.2)
			end
			bert:addPoint()
			pos = bert:getPosition()
			rateCur = rateCur + fg.fracSum(pos.x, pos.y, pos.z, 1, 1)
			bert:yaw(rateCur)
			bert:move(stepLength)
			distance = distance + self.stepLength
			--stepLength = stepLength * .7
		end
		bert:setScale(0.001)
		bert:endCylinder(10)
	end

	return obj
end

function new_holdfast(length,width,n,meander)
	local obj = {
		meanLength  = length,
		rootWidth   = width,
		numRoots    = n,
		rootMeander = meander,

		roots = {}
	}

    obj.build = function(self,donatello)
		for i = 1, self.numRoots, 1 do
			-- create a root
			length = fg.randomN(self.meanLength,.2)
			self.roots[i] = new_root(length,self.rootWidth,meander)

            donatello:pushState()

            donatello:roll(math.pi / self.numRoots * 2 * i)
			self.roots[i]:build(donatello)

			donatello:popState()
		end
	end

	return obj
end

function new_stalk(length,width,n)
	local obj = {
		numBeads = n,
		beadWidth = width,
		beadLength = length,

		dl = length * .1,
		dw = width * .5,

		bendy = 0.05
	}

	obj.build = function(self, bert)
	  bert:setCarrierMode(0)

	  -- Create the cylinder
	  width = self.beadWidth
	  length = self.beadLength
	  bert:setScale(self.beadWidth)
	  bert:beginCylinder()
	  bert:move(self.dl)
  	  --bert:setScale(width)

	  for i = 2, self.numBeads, 1 do
	  	bert:addPoint(7)

        bert:yaw(self.bendy)
--		bert:pitch(fg.random(-.07,.07))
		bert:move(self.dl * .5)
		bert:setScale(self.dw)
		bert:addPoint(7)
		bert:move(self.dl * .5)

	  	width = fg.randomN(self.beadWidth, self.beadWidth * .03)
	  	length = fg.randomN(self.beadLength, self.beadLength * .2)
		bert:setScale(width)
		bert:addPoint(7)
		bert:move(length)
		bert:setScale(width)
	  end
	  bert:endCylinder()
	end

	obj.update = function(self,dt)
	end

	return obj
end

function new_cylax(width,length,n)
	local obj = {
		baseWidth = width,
		length = length,
		numBumps = n,

		outerS = 1.7,
		innerS = 0.6,
		innerRat = 0.6
	}

	obj.build = function(self, bert)
	    bert:pushState()
		
		-- create cross section
		bert:setFrame(fg.vec3(1,0.,0.),fg.vec3(0.,1.,0.),fg.vec3(0.,0.,1.))
		bert:setStiffness(self.outerS,self.outerS)
		bert:beginCrossSection()
		y = self.innerRat * math.sin((.5) * 2 * math.pi / self.numBumps)
		x = self.innerRat * math.cos((.5) * 2 * math.pi / self.numBumps)

		dy = math.cos((.5) * 2 * math.pi / self.numBumps)
		dx = -math.sin((.5) * 2 * math.pi / self.numBumps)

		bert:setFrame(fg.vec3(x,y,0),fg.vec3(dx,dy,0),fg.vec3(0,0,1))
		bert:setStiffness(self.innerS,self.innerS)
		bert:addPoint()

    	i = 1
		while (i < self.numBumps) do
			y = math.sin(i * 2 * math.pi / self.numBumps)
			x = math.cos(i * 2 * math.pi / self.numBumps)

			dy = math.cos(i * 2 * math.pi / self.numBumps)
			dx = -math.sin(i * 2 * math.pi / self.numBumps)

			bert:setFrame(fg.vec3(x,y,0),fg.vec3(dx,dy,0),fg.vec3(0,0,1))
			bert:setStiffness(self.outerS,self.outerS)
			bert:addPoint()

			y = self.innerRat * math.sin((i+.5) * 2 * math.pi / self.numBumps)
			x = self.innerRat * math.cos((i+.5) * 2 * math.pi / self.numBumps)

			dy = math.cos((i+.5) * 2 * math.pi / self.numBumps)
			dx = -math.sin((i+.5) * 2 * math.pi / self.numBumps)

			bert:setFrame(fg.vec3(x,y,0),fg.vec3(dx,dy,0),fg.vec3(0,0,1))
			bert:setStiffness(self.innerS,self.innerS)
			bert:addPoint()

			 i = i + 1
		end

		cs = bert:endCrossSection()

		-- create a cylinder
		bert:popState()

    	bert:setCrossSection(0)

		bert:beginCylinder()
		bert:move(1)
    	bert:setCrossSection(cs)
		bert:setScale(1.8)
		bert:addPoint(7)
		bert:move(1)
		bert:setScale(0.9)
		bert:addPoint(7)
		bert:move(0.6)
		bert:setScale(0.6)
		bert:addPoint(7)
		bert:move(0.4)
		bert:setScale(0.1)
		bert:setCrossSection(0)
		bert:endCylinder(7)
	end

	return obj
end
