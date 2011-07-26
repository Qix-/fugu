
module(...,package.seeall)

local m = {}
local node = {}
function setup()
	-- Params for cross section
	numBumps = 5
	innerRat = .7
	outerS = .5
	innerS = .7

	-- Scale params

	-- Node for the mesh
	node = fg.node() -- blank transform node
end

local t = 0
function update(dt)
	t = t + dt

	innerRat = math.sin( t )

    -- Our friend bert
	bert = fg.turtle()
	bert:pushState()

	-- create cross section

	bert:setFrame(fg.vec3(1.,0.,0.),fg.vec3(0.,1.,0.),fg.vec3(0.,0.,1.))
	bert:setStiffness(outerS,outerS)
	bert:beginCrossSection()
	y = innerRat * math.sin((.5) * 2 * math.pi / numBumps)
	x = innerRat * math.cos((.5) * 2 * math.pi / numBumps)

	dy = math.cos((.5) * 2 * math.pi / numBumps)
	dx = -math.sin((.5) * 2 * math.pi / numBumps)

	bert:setFrame(fg.vec3(x,y,0),fg.vec3(dx,dy,0),fg.vec3(0,0,1))
	bert:setStiffness(innerS,innerS)
	bert:addPoint()

    i = 1
	while (i < numBumps) do
		y = math.sin(i * 2 * math.pi / numBumps)
		x = math.cos(i * 2 * math.pi / numBumps)

		dy = math.cos(i * 2 * math.pi / numBumps)
		dx = -math.sin(i * 2 * math.pi / numBumps)

		bert:setFrame(fg.vec3(x,y,0),fg.vec3(dx,dy,0),fg.vec3(0,0,1))
		bert:setStiffness(outerS,outerS)
		bert:addPoint()

		y = innerRat * math.sin((i+.5) * 2 * math.pi / numBumps)
		x = innerRat * math.cos((i+.5) * 2 * math.pi / numBumps)

		dy = math.cos((i+.5) * 2 * math.pi / numBumps)
		dx = -math.sin((i+.5) * 2 * math.pi / numBumps)

		bert:setFrame(fg.vec3(x,y,0),fg.vec3(dx,dy,0),fg.vec3(0,0,1))
		bert:setStiffness(innerS,innerS)
		bert:addPoint()

		 i = i + 1
	end

	cs = bert:endCrossSection()

	-- create a cylinder
	bert:setFrame(fg.vec3(0.,0.,0.),fg.vec3(0.,0.,1.),fg.vec3(0.,1.,0.))

    bert:setCrossSection(1)

	bert:setScale(0.1)
	bert:beginCylinder()
	bert:move(1.)
	bert:setScale(1.)
	bert:addPoint()
	bert:move(1.)
	bert:setScale(0.4)
	bert:endCylinder()
	m = bert:getMesh(10,40)
	fgu:addMesh(m)
end


