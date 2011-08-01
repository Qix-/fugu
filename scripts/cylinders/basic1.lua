
module(...,package.seeall)

local m = {}
function setup()
	bert = fg.turtle()
	bert:beginCylinder()
	bert:move(10.)
	bert:yaw(math.pi / 6)
	bert:addPoint()
	bert:yaw(math.pi / 6)
	bert:move(10.)
    bert:endCylinder()
	m = bert:getMesh()
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt
end


