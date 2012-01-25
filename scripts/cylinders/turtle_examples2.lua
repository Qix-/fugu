
module(...,package.seeall)

local m = {}
function setup()
	bert = fg.turtle()

    -- Create a cross section
    bert:pushState()
    bert:setFrame(vec3(1.5,0,0),vec3(0.,1.,0),vec3(0,0,1))
	bert:setStiffness(0.,0.)
	bert:yaw(-math.pi*.25)
	bert:beginCrossSection()
	bert:move(1.5)
	bert:addPoint()
	bert:yaw(-math.pi*.5)
	bert:move(1.5)
	bert:yaw(-math.pi*.5)
	bert:addPoint()
	bert:move(1.5)
	bert:yaw(-math.pi*.5)
	bert:endCrossSection()
    bert:popState()

    -- Create a cylinder
	bert:beginCylinder()
    bert:setCrossSection(0)
    bert:yaw(math.pi*.25)
	bert:move(10.)
    bert:yaw(math.pi*.25)
    bert:addPoint(5)
    bert:yaw(math.pi*.25)
	bert:move(10.)
    bert:yaw(math.pi*.25)
    bert:setCrossSection(1)
    bert:endCylinder(5)

	--bert:setCrossSection(1)
	--bert:beginCylinder()

	--bert:move(1)
    --bert:addPoint(3)

	--bert:move(1)
	--bert:setCrossSection(0)
    --bert:setScale(1)
    --bert:addPoint(3)

	--bert:move(1)
    --bert:addPoint(3)

	--bert:move(1)
    --bert:setScale(.2)
    --bert:endCylinder(3)

	m = bert:getMesh()
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt
end


