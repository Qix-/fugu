
module(...,package.seeall)

local m = {}
function setup()
    local n = 15

	bert = fg.turtle()

    bert:pushState()

    bert:setFrame(vec3(.5,-.5,0.),vec3(0.,1.,0.),vec3(0.,0.,1.))
	bert:setStiffness(0.,0.)

	bert:beginCrossSection()
	bert:move(1.)
	bert:addPoint()
	bert:yaw(-math.pi*.5)
	bert:move(1.)
	bert:yaw(-math.pi*.5)
	bert:addPoint()
	bert:move(1.)
	bert:yaw(-math.pi*.5)
	bert:endCrossSection()

    bert:popState()

    bert:pitch(-.5*math.pi)

        local stepLength = 1.
		local distance = 0
		local rateCur = .05 
		local stepLength = 1.
        local rootLength = 10
        local rootMeander = .7
        local width = 1

		--bert:setScale(width)
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
		--bert:setScale(0.001)
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


