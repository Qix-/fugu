module(...,package.seeall)

local m = {}
local node = {}

function setup()
	hold_fast = new_holdfast(2,.3,46,.01)

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
		stepLength  = length / 8
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

	local points = spherePoints(n)

	obj.build = function(self,donatello)
		for i = 1, self.numRoots do
			-- create a root at points[i]
			length = randomN(self.meanLength,.2)
			self.roots[i] = new_root(length,self.rootWidth,meander)

			local p = points[i]
			local dir = normalise(p)
			local up = perp(dir)
			donatello:pushState()
				donatello:setFrame(p*.3,dir,up)
				--donatello:yaw(math.pi/self.numRoots * 2 * i)
				--donatello:roll(math.pi / self.numRoots * 2 * i + .03)
				self.roots[i]:build(donatello)
			donatello:popState()
		end
	end
	return obj
end

-- returns a normalised vector perpendicular to vec3 v
function perp(v)
	if (v.x<v.y and v.x<v.z) then return normalise(cross(v,vec3(1,0,0)))
	elseif (v.y<v.x and v.y<v.z) then return normalise(cross(v,vec3(0,1,0)))
	else return normalise(cross(v,vec3(0,0,1))) end
end

--[[	
 "Distributing many points on a sphere" by E.B. Saff and A.B.J. Kuijlaars,
  Mathematical Intelligencer 19.1 (1997) 5--11
--]]
--
--[[
for k=1 to N do
       h = -1 + 2*(k-1)/(N-1)
       theta[k] = arccos(h)
       if k=1 or k=N then phi[k] = 0
       else phi[k] = (phi[k-1] + 3.6/sqrt(N*(1-h^2))) mod (2*pi)
    endfor

  In Cartesian coordinates the required point on a sphere of radius 1 is
     (cos(theta)*sin(phi), sin(theta)*sin(phi), cos(phi))
--]]

function spherePoints(N)
	local points = {}
	local	theta = {}
	local phi = {}
	for k=1,N do
		local h = -1 + 2*(k-1)/(N-1)
		theta[k] = math.acos(h)
		if (k==1 or j==N) then 
			phi[k] = 0
		else 
			phi[k] = math.mod(phi[k-1] + 3.6/math.sqrt(N*(1-h*h)),2*math.pi)	
		end
		points[k] = vec3(math.cos(theta[k])*math.sin(phi[k]),math.sin(theta[k])*math.sin(phi[k]),math.cos(phi[k]))
	end
	return points
end
