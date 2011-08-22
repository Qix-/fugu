module(...,package.seeall)

-- script to demonstrate the phylotaxis functions
-- jonmc, August 17, 2011

local m = {}
local node = {}

local A = 4		-- sphere radius
local B = 10    	-- prolate
local elemR = 0.5 	-- element radius
local N = 550		-- number of elements
local PHI = 2.39982772	-- golden ratio
local SPA = 0.7554	-- segment area

function setup()
	hold_fast = new_holdfast(2,math.sqrt(SPA/math.pi),N,.01)

	donatello = fg.turtle()
	donatello:pushState()
	donatello:pitch(math.pi/2)
	hold_fast:build(donatello)
	donatello:popState()

	node = fg.meshnode(donatello:getMesh())
	fgu:add(node)
end

local t = 0
function update(dt) end


function new_root(length,width,meander)
	local obj = {
		rootLength  = length,
		rootWidth   = width,
		rootMeander = meander,
		stepLength  = length / 4
	}

	obj.build = function(self,bert)
		-- Create the cylinder

		-- First the turn
		bert:setScale(self.rootWidth)
		bert:setStiffness(0.3,0.3)
		bert:beginCylinder()
		--[[
		bert:move(self.rootWidth)
		bert:pitch(math.pi * .25)
		bert:move(self.rootWidth)
		bert:setScale(self.rootWidth * .7)
		bert:addPoint(10)
		bert:move(self.rootWidth)
		bert:pitch(math.pi * .25)
		bert:move(self.rootWidth)		
		bert:setScale(self.rootWidth * .5)
		--]]		

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
			rateCur = rateCur + self.rootMeander --  * fracSum(pos.x, pos.y, pos.z, 1, 1)
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
		for i = 1, self.numRoots do
			-- create a root at points[i]
			length = randomN(self.meanLength,.2)
			self.roots[i] = new_root(length,self.rootWidth,meander)

			donatello:roll(PHI)
			donatello:pushState()
			local elipH = findEllipseH(A, B, SPA * i);
			donatello:pitch(math.pi)
			donatello:move(elipH)
			donatello:pitch(math.pi/4)
			donatello:move(A * math.sqrt(1 - ((elipH * elipH)/(B * B))))
			donatello:pitch(getAngleFromH(A, B, elipH))
			donatello:pushState()
			self.roots[i]:build(donatello)
			donatello:popState()
			donatello:popState()
		end
	end
	return obj
end
