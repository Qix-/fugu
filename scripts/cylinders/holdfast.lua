module(...,package.seeall)

local m = {}
local node = {}

function setup()

	b = new_holdfast(10.,2.,6.,.5)

    donatello = fg.turtle()
	donatello:pitch(-math.pi*.5)

	b:build(donatello)

	node = fg.meshnode(donatello:getMesh())
	fgu:add(node)

end

local t = 0
function update(dt)

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
			self.roots[i] = new_root(length,self.rootWidth,self.rootMeander)

            donatello:pushState()

            donatello:roll(math.pi / self.numRoots * 2 * i + .03)
			self.roots[i]:build(donatello)

			donatello:popState()
		end
	end

	return obj
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
		bert:addPoint(2)
		bert:move(self.rootWidth)
		bert:pitch(math.pi * .25)
		bert:move(self.rootWidth)
		bert:setScale(self.rootWidth * .5)
		
		-- Now the wiggly bit
		distance = 0
		rateCur = 0
		stepLength = self.stepLength
		bert:addPoint(2)
		bert:move(self.stepLength)
		distance = distance + stepLength
--
		while (distance < self.rootLength) do
			if (distance > .6 * self.rootLength) then
			--	bert:setScale(0.2)
			end
			bert:yaw(rateCur/2)
			bert:addPoint(2)
			pos = bert:getPosition()
			rateCur = rateCur + self.rootMeander * fracSum(pos.x, pos.y, pos.z, 1, 1)
			bert:yaw(rateCur/2)
			bert:move(stepLength)
			distance = distance + self.stepLength
			--stepLength = stepLength * .7
		end
		bert:setScale(0.001)
		bert:yaw(rateCur/2)
		bert:endCylinder(2)
	end

	return obj
end
