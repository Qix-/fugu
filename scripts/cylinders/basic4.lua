
module(...,package.seeall)

local m = {}
function setup()
	bert = fg.turtle()
	bert:pushState()

	bert:beginCylinder()
	bert:move(10.)
	bert:yaw(math.pi / 6)
	bert:addPoint()
	bert:yaw(math.pi / 6)
	bert:move(10.)
	bert:setScale(0.5)
	bert:addPoint()
	bert:yaw(math.pi / 6)
	bert:move(10.)
	bert:addPoint()
	bert:move(2.0)
	bert:setScale(0.1)
    bert:endCylinder()

    bert:popState()

	bert:beginCylinder()
	bert:pitch(1.)
	bert:move(10.)
	bert:addPoint()
	bert:roll(0.5)
	bert:yaw(.8)
	bert:move(13.)
	bert:addPoint()
	bert:setScale(0.4)
	bert:endCylinder()
	m = bert:getMesh()
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt
end


