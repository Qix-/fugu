
module(...,package.seeall)

local m = {}
function setup()
    local n = 8
	bert = fg.turtle()
	bert:beginCylinder()
    bert:yaw(math.pi*.25)
	bert:move(10.)
    bert:yaw(math.pi*.25)
    bert:endCylinder(n)
	m = bert:getMesh()
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt
end


