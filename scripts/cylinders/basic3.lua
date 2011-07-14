
module(...,package.seeall)

local m = {}
function setup()
	bert = fg.turtle()
	bert:beginCylinder()
	bert:move(1.)
	bert:roll(math.pi)
	bert:addPoint()
	bert:move(1.)
	bert:roll(-math.pi)
    bert:endCylinder()
	m = bert:getMesh(0,4,50)
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt
end


