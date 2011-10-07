
module(...,package.seeall)

local m = {}
function setup()
	i = 10
	bert = fg.turtle()
	bert:setCarrierMode(0)
	bert:beginCylinder()
	bert:yaw(math.pi/4);
	bert:move(10.);
	bert:yaw(math.pi/4);
	bert:addPoint(i);
	bert:yaw(math.pi/4);
	bert:move(10.);
	bert:yaw(math.pi/4);
	bert:addPoint(i);
	bert:yaw(math.pi/4);
	bert:move(10.);
	bert:yaw(math.pi/4);
	bert:addPoint(i);
    bert:yaw(math.pi/4);
    bert:move(10.);
    bert:yaw(math.pi/4);
    bert:endCylinder(i)
	m = bert:getMesh()
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt
end


