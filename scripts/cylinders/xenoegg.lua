
module(...,package.seeall)

local m = {}
local node = {}
local first = true
function setup()
	-- Params for cross section
	numBumps = 7
	innerRat = .7
	outerS = .5
	innerS = .7

	-- Scale params

end

local t = 0
function update(dt)
	t = t + dt

	innerRat = .5 * math.sin( t ) * math.sin( t ) + .8

    -- Our friend bert
	bert = fg.turtle()
	bert:pushState()
	bert:setCarrierMode(0)

	-- create cross section

	bert:setFrame(fg.vec3(1,0.,0.),fg.vec3(0.,1.,0.),fg.vec3(0.,0.,1.))
	bert:setStiffness(outerS,outerS)
	bert:beginCrossSection()
	y = 1*innerRat * math.sin((.5) * 2 * math.pi / numBumps)
	x = 1*innerRat * math.cos((.5) * 2 * math.pi / numBumps)

	dy = math.cos((.5) * 2 * math.pi / numBumps)
	dx = -math.sin((.5) * 2 * math.pi / numBumps)

	bert:setFrame(fg.vec3(x,y,0),fg.vec3(dx,dy,0),fg.vec3(0,0,1))
	bert:setStiffness(innerS,innerS)
	bert:addPoint()

    i = 1
	while (i < numBumps) do
		y = 1*math.sin(i * 2 * math.pi / numBumps)
		x = 1*math.cos(i * 2 * math.pi / numBumps)

		dy = math.cos(i * 2 * math.pi / numBumps)
		dx = -math.sin(i * 2 * math.pi / numBumps)

		bert:setFrame(fg.vec3(x,y,0),fg.vec3(dx,dy,0),fg.vec3(0,0,1))
		bert:setStiffness(outerS,outerS)
		bert:addPoint()

		y = 1*innerRat * math.sin((i+.5) * 2 * math.pi / numBumps)
		x = 1*innerRat * math.cos((i+.5) * 2 * math.pi / numBumps)

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

    bert:setCrossSection(0)

	bert:setScale(0.4)
	bert:beginCylinder()
	bert:move(1)
    bert:setCrossSection(cs)
	bert:setScale(1.5)
	bert:addPoint()
	bert:move(1)
	bert:setScale(0.9)
    bert:setCrossSection(0)
	bert:addPoint()
	bert:move(0.6)
	bert:setScale(0.6)
	bert:addPoint()
	bert:move(0.4)
	bert:setScale(0.1)
	bert:endCylinder()

	m = bert:getMesh()
	if (first) then
		-- Node for the mesh
		node = fg.meshnode(m) -- blank transform node
		fgu:add(node)
		first = false
	end
	node:setMesh(m)
end


