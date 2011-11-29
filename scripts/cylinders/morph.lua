
module(...,package.seeall)

local m = {}
function setup()
	bert = fg.turtle()

    -- Create a cross section
    bert:setFrame(vec3(.5,-.5,0.),vec3(0.,1.,0.),vec3(0.,0.,1.))
	bert:setStiffness(0.,0.)
	bert:yaw(-math.pi*.25)
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

	bert:setCarrierMode(0)
	bert:beginCylinder()
	bert:move(5.)
	bert:setCrossSection(1)
    bert:endCylinder(3)

	m = bert:getMesh()
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt
end


