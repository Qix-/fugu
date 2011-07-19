
module(...,package.seeall)

local m = {}
function setup()
	bert = fg.turtle()
	bert:beginCylinder()
	bert:yaw(math.pi/4);
	bert:move(1.);
	bert:yaw(math.pi/4);
	bert:addPoint();
	bert:yaw(math.pi/4);
	bert:move(1.);
	bert:yaw(math.pi/4);
	bert:addPoint();
	bert:yaw(math.pi/4);
	bert:move(1.);
	bert:yaw(math.pi/4);
	bert:addPoint();
    bert:yaw(math.pi/4);
    bert:move(1.);
    bert:yaw(math.pi/4);
    bert:endCylinder()
	m = bert:getMesh(10,12)
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt
end


