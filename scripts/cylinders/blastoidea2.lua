module(...,package.seeall)

local m = {}
local node = {}

function setup()

--	-- roots
--	root = new_root(15,.5,.5)
--
--	don = turtle()
--	root:build(don)
--	m[1] = don:getMesh()
--	node[1] = meshnode(m[1])

    -- stalk
	stalk = new_stalk(.35,.7,15)

    -- holdfast
	hold_fast = new_holdfast(4.,.7,6,.7)

	-- calyx
	calyx = new_cylax(.9,.7,5)


    -- brachioles
    donatello = fg.turtle()

    donatello:pushState()

	donatello:pitch(math.pi)
	hold_fast:build(donatello)

	donatello:popState()
	stalk:build(donatello)

	donatello:pushState()

	calyx:build(donatello)

	donatello:popState()

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
			rateCur = rateCur + self.rootMeander * fracSum(pos.x, pos.y, pos.z, 1, 1)
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
			length = randomN(self.meanLength,.2)
			self.roots[i] = new_root(length,self.rootWidth,meander)

            donatello:pushState()

            donatello:roll(math.pi / self.numRoots * 2 * i + .03)
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

		bendy = 0.01
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
--		bert:pitch(random(-.07,.07))
		bert:move(self.dl * .5)
		bert:setScale(self.dw)
		bert:addPoint(7)
		bert:move(self.dl * .5)

	  	width = randomN(self.beadWidth, self.beadWidth * .03)
	  	length = randomN(self.beadLength, self.beadLength * .2)
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

		outerS = 1.9,
		innerS = 1.2,
		innerRat = 0.54
	}

	obj.build = function(self, bert)
	    bert:pushState()
		
		-- create cross section
		bert:setFrame(vec3(1,0.,0.),vec3(0.,1.,0.),vec3(0.,0.,1.))
		bert:setStiffness(self.outerS,self.outerS)
		bert:beginCrossSection()
		y = self.innerRat * math.sin((.5) * 2 * math.pi / self.numBumps)
		x = self.innerRat * math.cos((.5) * 2 * math.pi / self.numBumps)

		dy = math.cos((.5) * 2 * math.pi / self.numBumps)
		dx = -math.sin((.5) * 2 * math.pi / self.numBumps)

		bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))
		bert:setStiffness(self.innerS,self.innerS)
		bert:addPoint()

    	i = 1
		while (i < self.numBumps) do
			y = math.sin(i * 2 * math.pi / self.numBumps)
			x = math.cos(i * 2 * math.pi / self.numBumps)

			dy = math.cos(i * 2 * math.pi / self.numBumps)
			dx = -math.sin(i * 2 * math.pi / self.numBumps)

			bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))
			bert:setStiffness(self.outerS,self.outerS)
			bert:addPoint()

			y = self.innerRat * math.sin((i+.5) * 2 * math.pi / self.numBumps)
			x = self.innerRat * math.cos((i+.5) * 2 * math.pi / self.numBumps)

			dy = math.cos((i+.5) * 2 * math.pi / self.numBumps)
			dx = -math.sin((i+.5) * 2 * math.pi / self.numBumps)

			bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))
			bert:setStiffness(self.innerS,self.innerS)
			bert:addPoint()

			 i = i + 1
		end

		cs = bert:endCrossSection()

		-- create a cylinder
		bert:popState()
		bert:pushState()

    	bert:setCrossSection(0)
		bert:setScale(self.baseWidth)

		bert:beginCylinder()
		bert:move(self.length)
    	bert:setCrossSection(cs)
		bert:setScale(1.8*self.baseWidth)
		bert:addPoint(7)
		bert:move(self.length)
		bert:setScale(0.9*self.baseWidth)
		bert:addPoint(7)
		bert:move(0.6*self.length)
		bert:setScale(0.6*self.baseWidth)
		bert:addPoint(7)
		bert:move(0.4*self.length)
		bert:setScale(0.1*self.baseWidth)
		bert:setCrossSection(0)
		bert:endCylinder(7)

		bert:popState()

		bert:move(0.7)

	  	for i = 1, self.numBumps, 1 do
			bert:pushState()

			brachioles = new_brachioles_bunch(.03,.2,7,8)

			pos = vec3(.7*math.cos((i + .5) * 2 * math.pi / self.numBumps), 
			              .7*math.sin((i + .5) * 2 * math.pi / self.numBumps),
						  0)
			up = vec3(-math.cos((i+.5) * 2 * math.pi / self.numBumps),
						   -math.sin((i+.5) * 2 * math.pi / self.numBumps),
						   0)

			donatello:setFrameRel( pos, vec3(0,0,1), up )

			brachioles:build(donatello)

			bert:popState()
		end

	end

	return obj
end

function new_brachioles(width,length,n)
	local obj = {
		segWidth = width,
		segLength = length,
		numSegs = n,

		l = 0.2,
		dl = length * .2,
		meander = 0.08,
		meander2 = 0.007
	}

	obj.build = function(self, bert)
	    bert:pushState()
		
		-- create cross section
		bert:setStiffness(.3,.3)
		bert:setFrame(vec3(1+self.l,0.,0.),vec3(0.,1.,0.),vec3(0.,0.,1.))
		bert:beginCrossSection()

		bert:setFrame(vec3(0,1,0),vec3(-1,0.,0.),vec3(0.,0.,1.))
		bert:addPoint()

		bert:setFrame(vec3(-1-self.l,0,0),vec3(0,-1,0),vec3(0.,0.,1.))
		bert:addPoint()

		bert:setFrame(vec3(0,-1,0),vec3(1,0,0),vec3(0.,0.,1.))
		bert:addPoint()
		cs1 = bert:endCrossSection()

		-- create cross section
		bert:setFrame(vec3(1-self.l,0.,0.),vec3(0.,1.,0.),vec3(0.,0.,1.))
		bert:beginCrossSection()

		bert:setFrame(vec3(0,1,0),vec3(-1,0.,0.),vec3(0.,0.,1.))
		bert:addPoint()

		bert:setFrame(vec3(-1+self.l,0,0),vec3(0,-1,0),vec3(0.,0.,1.))
		bert:addPoint()

		bert:setFrame(vec3(0,-1,0),vec3(1,0,0),vec3(0.,0.,1.))
		bert:addPoint()
		cs2 = bert:endCrossSection()

		theta = 0
		theta2 = 0

		-- create a cylinder
		width = self.segWidth
		length = self.segLength

		bert:popState()

    	bert:setCrossSection(0)
		bert:setScale(width)
		bert:setStiffness(.3,.3)

		bert:beginCylinder()

		notScaled = true

	  	for i = 1, self.numSegs, 1 do
			width = randomN(self.segWidth, self.segWidth * .1)
			length = randomN(self.segLength, self.segLength * .1)

			pos = bert:getPosition()
			theta = self.meander * fracSum(pos.x, pos.y, pos.z, 3, 1.5)
			theta2 = self.meander2 * fracSum(pos.x, pos.y, pos.z, 1, 1)

 			bert:yaw(theta)
			bert:pitch(theta)
			bert:move(length)
			bert:addPoint(5)

        	bert:move(self.dl * .3)
    		bert:setCrossSection(cs1)
			bert:addPoint(5)

			bert:move(self.dl * .2)
    		bert:setCrossSection(cs2)
			bert:addPoint(5)

        	bert:move(self.dl * .2)
    		bert:setCrossSection(cs1)
			bert:addPoint(5)

        	bert:move(self.dl * .3)
    		bert:setCrossSection(0)
			bert:addPoint(5)

--			length = length * .9
		end

        bert:setScale(self.segWidth)
		bert:move(self.dl)
		bert:addPoint()

        bert:setScale(0.1 * self.segWidth)
		bert:move(self.segLength * .5)
		bert:endCylinder(12)
	end

	return obj
end

function new_brachioles_bunch(width,length,nsegs,n)
	local obj = {
		segWidth = width,
		segLength = length,
		numSegs = nsegs,
		numBs = n
	}

	obj.build = function(self, bert)
    	numSegs = self.numSegs * .5
		segInc  = self.numSegs / self.numBs

		width = self.segWidth * .5
		widthInc  = self.segWidth / self.numBs

		length = self.segLength * .7
		lengthInc  = self.segLength * .45 / self.numBs

		notSwaped = true

		--place some brachioles
	  	for i = 1, self.numBs, 1 do
			if (i > self.numBs * .5 and notSwaped) then
				widthInc = -widthInc
				segInc = -segInc
				lengthInc = -lengthInc
				notSwaped = false
			end

			bert:pushState()

			bert:pitch(math.pi * .5)
			b = new_brachioles(width, length, numSegs)
			b:build(bert)

			bert:popState()
			bert:move(width * 3)

			numSegs = numSegs + segInc
			width = width + widthInc
			length = length + lengthInc
		end

	end

	return obj
end
