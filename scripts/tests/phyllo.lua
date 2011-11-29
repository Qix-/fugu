module(...,package.seeall)

-- script to demonstrate the phylotaxis functions
-- jonmc, August 17, 2011

local m = {}
local node = {}

local A = 7		-- sphere radius
local B = 1    	-- prolate
local elemR = 0.5 	-- element radius
local N = 100		-- number of elements
local PHI = 2.39982772	-- golden ratio
local SPA = 0.7554	-- segment area

function setup()
	donatello = fg.turtle()

	node = fg.meshnode(donatello:getMesh())
	fgu:add(node)
end

local t = 0
function update(dt) 
    t = t + dt

    local phi = lerp(-.2,.2,t)
    phi = phi + PHI
	hold_fast = new_holdfast(2,math.sqrt(SPA/math.pi),N,.01,phi)

	donatello = fg.turtle()
	donatello:pushState()
	donatello:pitch(math.pi/2)
	hold_fast:build(donatello)
	donatello:popState()

    node:setMesh(donatello:getMesh())
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

		-- First the turn
		bert:setScale(self.rootWidth)
		bert:setStiffness(0.3,0.3)
        bert:setCarrierMode(0)
		bert:beginCylinder()
        bert:move(self.rootLength)
		bert:setScale(0.001)
		bert:endCylinder(10)
	end

	return obj
end

function new_holdfast(length,width,n,meander,phi)
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

			donatello:roll(phi)
			donatello:pushState()
			local elipH = find_ellipse_h(A, B, SPA * i);
			donatello:pitch(math.pi)
			donatello:move(elipH)
			donatello:pitch(math.pi/4)
			donatello:move(A * math.sqrt(1 - ((elipH * elipH)/(B * B))))
			donatello:pitch(get_angle_from_h(A, B, elipH))
			donatello:pushState()
			self.roots[i]:build(donatello)
			donatello:popState()
			donatello:popState()
		end
	end
	return obj
end
