module(...,package.seeall)

local m = {}
local node = {}
local stalk = {}
function setup()
	-- The stalk
   	stalk = new_stalk(15,.4,.3,.7)

	m[1] = stalk:getMesh()
	-- Node for the mesh
	node[1] = fg.meshnode(m[1])
	fgu:add(node[1])
	-- roots
	holdfast = new_holdfast(4,2,7,.5)
	m[2] = holdfast:getMesh()
	node[2] = fg.meshnode(m[2])
	fgu:add(node[2])
	--node[1].transform:setTranslate(0,0,0.7)		
end

local t = 0
function update(dt)
	t = t + dt
	stalk.update(dt)
end

function new_holdfast(width,length,n,meander)
	local obj = {
		meanLength  = length,
		rootWidth   = width,
		numRoots    = n,
		rootMeander = meander,

		roots = {}
	}

    obj.getMesh = function(self)
		donatello = fg.turtle()

        donatello:pitch(math.pi)
		for i = 1, self.numRoots, 1 do
			-- create a root
			length = self.meanLength * random(.8,1.2)
			self.roots[i] = new_root(length,self.rootWidth*.5,meander)

            donatello:pushState()

			--donatello:move(-self.rootWidth * .05)
			donatello:setScale(self.rootWidth)
			donatello:beginCylinder()
			donatello:move(self.rootWidth*.2)
			donatello:setStiffness(.4,.4)
			donatello:addPoint()
			donatello:pitch(math.pi * .5)
			donatello:yaw(math.pi * 2 / self.numRoots * i)
			donatello:move(self.rootWidth*.2)
			--donatello:move(self.rootWidth * .05)
			donatello:setScale(self.rootWidth*.5)
			donatello:setStiffness(.4,.4)
			donatello:endCylinder()
			self.roots[i]:build(donatello)

			donatello:popState()
		end

		return donatello:getMesh()
	end

	return obj
end

function new_root(length,width,meander)
	local obj = {
		rootLength  = length,
		rootWidth   = width,
		rootMeander = meander,
		stepLength  = length / 4
	}

	obj.build = function(self,bert)
		-- Create the cylinder
		distance = 0
		bert:setScale(self.rootWidth)
		bert:beginCylinder()
		bert:move(self.stepLength)
		distance = distance + self.stepLength

		while (distance < self.rootLength) do
			bert:addPoint()
			theta = random(-meander,meander)
			bert:yaw(theta)
			bert:move(self.stepLength)
			distance = distance + self.stepLength
		end
		bert:setScale(0.001)
		bert:endCylinder()
	end

	return obj
end

function new_stalk(n,width,length,meander)
	local obj = {
		numBeads = n,
		beadWidth = width * 10,
		beadLength = length,
		meanderParam = meander,

		dl = length * .25,
		dw = width * 9,

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
	  	bert:addPoint()

        bert:yaw(self.bendy)
		bert:pitch(random(-.07,.07))
		bert:move(self.dl * .5)
		bert:setScale(self.dw)
		bert:addPoint()
		bert:move(self.dl * .5)

	  	width = self.beadWidth * random(0.91,1.09)
	  	length = self.beadLength * random(0.91,1.09)
		bert:setScale(width)
		bert:addPoint()
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
