
module(...,package.seeall)

local m = {}
function setup()
	bert = fg.turtle()
	bert:beginCylinder()
	bert:move(1.)
	bert:yaw(math.pi / 6)
	bert:addPoint()
	bert:yaw(math.pi / 6)
	bert:move(1.)
    bert:endCylinder()
	m = bert:getMesh(0,10,10)
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt
end


