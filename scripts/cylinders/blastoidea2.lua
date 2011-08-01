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
	stalk = new_stalk(.6,.5,15)
	m[1] = stalk:getMesh()
	node[1] = fg.meshnode(m[1])
	fgu:add(node[1])

    -- holdfast
	hold_fast = new_holdfast(5,.5,5,.1)
	m[2] = hold_fast:getMesh()
	node[2] = fg.meshnode(m[2])
	node[2].transform:setRotateRad(math.pi,1,0,0)
	fgu:add(node[2])

end

local t = 0
function update(dt)
	t = t + dt
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

    obj.getMesh = function(self)
		donatello = fg.turtle()

		for i = 1, self.numRoots, 1 do
			-- create a root
			length = fg.randomN(self.meanLength,.2)
			self.roots[i] = new_root(length,self.rootWidth,meander)

            donatello:pushState()

            donatello:roll(math.pi / self.numRoots * 2 * i)
			self.roots[i]:build(donatello)

			donatello:popState()
		end

		return donatello:getMesh()
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

		bendy = 0.07
	}

	obj.getMesh = function(self)
	  bert = fg.turtle()
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

	  	width = fg.randomN(self.beadWidth, 0.02)
	  	length = fg.randomN(self.beadLength,.09)
		bert:setScale(width)
		bert:addPoint(7)
		bert:move(length)
		bert:setScale(width)
	  end
	  bert:endCylinder()
	  return bert:getMesh()
	end

	obj.update = function(self,dt)
	end

	return obj
end
