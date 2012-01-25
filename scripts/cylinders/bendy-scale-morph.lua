
module(...,package.seeall)

local m = {}
function setup()
    local n = 15

	cs1 = new_bumpy_cs(5, 1.5, .2, 0, 1)
	cs1:init()

	cs2 = new_bumpy_cs(5, 1.5, .2, 0, 1)
	cs2:init()

	bert = fg.turtle()

	bert:pushState()
	cs1:build(bert)
	cs2:build(bert)
--	cs[2]:build(donatello)
--	cs[3]:build(donatello)
	bert:popState()

--    bert:pushState()
--
--    bert:setFrame(vec3(.5,-.5,0.),vec3(0.,1.,0.),vec3(0.,0.,1.))
--	bert:setStiffness(0.,0.)
--
--	bert:beginCrossSection()
--	bert:move(1.)
--	bert:addPoint()
--	bert:yaw(-math.pi*.5)
--	bert:move(1.)
--	bert:yaw(-math.pi*.5)
--	bert:addPoint()
--	bert:move(1.)
--	bert:yaw(-math.pi*.5)
--	bert:endCrossSection()
--
--    bert:popState()

    bert:pitch(-.5*math.pi)

        local stepLength = 1.
		local distance = 0
		local rateCur = .05 
		local stepLength = 1.
        local rootLength = 10
        local rootMeander = 1 
        local width = 1

        bert:setCrossSection(1)
		bert:setScale(width)
		bert:setStiffness(0.3,0.3)
		bert:beginCylinder()
		bert:move(width)
		--bert:setScale(width * .7)
		bert:addPoint(2)
		bert:move(width)
		--bert:setScale(width * .5)

		bert:addPoint(2)
		bert:move(stepLength)
		distance = distance + stepLength
--
		while (distance < rootLength) do
			if (distance > .6 * rootLength) then
			--	bert:setScale(0.2)
                bert:setCrossSection(0)
			end
			bert:yaw(rateCur/2)
			bert:addPoint(2)
			pos = bert:getPosition()
			rateCur = rateCur + rootMeander * fracSum(pos.x, pos.y, pos.z, 1, 1)
            print(fracSum(pos.x, pos.y, pos.z, 1, 1))
			bert:yaw(rateCur/2)
			bert:move(stepLength)
			distance = distance + stepLength
			--stepLength = stepLength * .7
		end
		bert:setScale(0.001)
		bert:yaw(rateCur/2)
		bert:endCylinder(2)

--	bert:setCarrierMode(1)
--    bert:setCrossSection(0)
--    bert:setStiffness(.3,.3)
--	bert:beginCylinder()
--    bert:pitch(math.pi*.125)
--	bert:move(10.6)
--    bert:pitch(math.pi*.105)
--    bert:addPoint(n)
--    bert:pitch(math.pi*.105)
--	bert:move(8.6)
--    bert:pitch(-math.pi*.15)
--    bert:endCylinder(n)
	m = bert:getMesh()
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt
end

function new_bumpy_cs(nbumps,size,noise,initAge,growTime)
    obj = {}

	obj.nbumps = nbumps
	obj.size = size
	obj.noise = noise

	obj.t = {}
	obj.s = {}

	obj.init = function(self)
		theta = 0
		thetaInc = math.pi / self.nbumps

		i = 1
		while (i <= self.nbumps * 2) do
			-- non bump
			self.t[i] = randomN(theta, thetaInc * self.noise)
			self.s[i] = randomN(1, self.noise)

			i = i + 1
			theta = theta + thetaInc

			-- bump
			self.t[i] = randomN(theta, thetaInc * self.noise)
			self.s[i] = randomN(self.size, (self.size - 1) * self.noise)

			i = i + 1
			theta = theta + thetaInc
		end
	end

	obj.build = function(self,bert)
		--local a = (self.size - 1) * smoothstep(0, self.gt, self.age)

		-- first point (not a bump)
		y = self.s[1] * math.sin(self.t[1])
		x = self.s[1] * math.cos(self.t[1])

		local dy = math.cos(self.t[1])
		local dx = -math.sin(self.t[1])

		bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))

		bert:beginCrossSection()

		-- add a bump
		y = self.s[2] * math.sin(self.t[2])
		x = self.s[2] * math.cos(self.t[2])

		local dy = math.cos(self.t[2])
		local dx = -math.sin(self.t[2])

		bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))

		i = 3
		while (i <= self.nbumps * 2) do
			bert:addPoint()

			-- add the next non bump
			y = math.sin(self.t[i])
			x = math.cos(self.t[i])

			dy = math.cos(self.t[i])
			dx = -math.sin(self.t[i])

			bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))
			bert:addPoint()

			i = i + 1

			-- add the next bump
			y = self.s[i] * math.sin(self.t[i])
			x = self.s[i] * math.cos(self.t[i])

			dy = math.cos(self.t[i])
			dx = -math.sin(self.t[i])

			bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))

			i = i + 1
		end

		return bert:endCrossSection()
	end

	return obj
end
