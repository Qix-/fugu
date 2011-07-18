
module(...,package.seeall)

local m = {}
function setup()
	bert = fg.turtle()

	bert:pushState()

    -- Create a cross section
--	bert:setStiffness(1.,1.)
	bert:pitch(-.5*math.pi)
	bert:yaw(.5)
	bert:beginCrossSection()
	bert:yaw(.5)
	bert:move(.5)
	bert:addPoint()
	bert:yaw(1)
	bert:move(.5)
	bert:addPoint()
	bert:yaw(1)
	bert:move(.5)
	bert:addPoint()
	bert:yaw(1)
	bert:move(.5)
	bert:yaw(.5)
	bert:endCrossSection()

	bert:popState()

    -- Create the cylinder
	bert:setCrossSection(1)
	bert:beginCylinder()
	bert:move(1.)
	bert:addPoint()
	bert:pitch(0.3)
	bert:move(1.)
	bert:setScale(0.4)
    bert:endCylinder()

	m = bert:getMesh(10,10)
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt
end


