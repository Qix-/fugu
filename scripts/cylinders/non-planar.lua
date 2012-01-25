module(...,package.seeall)

local array = {}
local node = {}
local cs = {}
local m = {}
function setup()
	donatello = fg.turtle()

	self = {
		w = 1.,
		t = 0.4,
		b = 0.
	}

	-- Some cs
	donatello:pushState()
	donatello:setStiffness( 0.3, 0.3 )

	donatello:setFrame( vec3( 1., 0., 0. ), vec3( 0., 1., 0. ), vec3( 0., 0., 1. ) )
	donatello:beginCrossSection()

	donatello:setFrame( vec3( 0., 1., 0. ), vec3( -1., 0., 0. ), vec3( 0., 0., 1. ) )
	donatello:addPoint(5)

	donatello:setFrame( vec3( -.7, 1., 0. ), vec3( -1., 0., 0. ), vec3( 0., 0., 1. ) )
	donatello:addPoint(5)

	donatello:setFrame( vec3( -2.5, 0., -2.5 ), vec3( 0., -1., 0. ), vec3( 0., 0., 1. ) )
	donatello:addPoint(5)

	donatello:setFrame( vec3( -.7, -1., 0. ), vec3( 1., 0., 0. ), vec3( 0., 0., 1. ) )
	donatello:addPoint(5)

	donatello:setFrame( vec3( 0., -1., 0. ), vec3( 1., 0., 0. ), vec3( 0., 0., 1. ) )
	donatello:endCrossSection()

	donatello:popState()
	donatello:pushState()
	donatello:setStiffness( 0.3, 0.3 )

	donatello:setFrame( vec3( 1., 0., 0. ), vec3( 0., 1., 0. ), vec3( 0., 0., 1. ) )
	donatello:beginCrossSection()

	donatello:setFrame( vec3( 0., 1., 0. ), vec3( -1., 0., 0. ), vec3( 0., 0., 1. ) )
	donatello:addPoint(5)

	donatello:setFrame( vec3( -3., 0., .0 ), vec3( 0., -1., 0. ), vec3( 0., 0., 1. ) )
	donatello:addPoint(5)

	donatello:setFrame( vec3( 0., -1., 0. ), vec3( 1., 0., 0. ), vec3( 0., 0., 1. ) )
	donatello:endCrossSection()

	donatello:popState()
	donatello:pushState()
	donatello:setStiffness( 0.3, 0.3 )

	donatello:setFrame( vec3( 1., 0., 0. ), vec3( 0., 1., 0. ), vec3( 0., 0., 1. ) )
	donatello:beginCrossSection()

	donatello:setFrame( vec3( 0., 1., 0. ), vec3( -1., 0., 0. ), vec3( 0., 0., 1. ) )
	donatello:addPoint(5)

	donatello:setFrame( vec3( -.7, 1., 0. ), vec3( -1., 0., 0. ), vec3( 0., 0., 1. ) )
	donatello:addPoint(5)

	donatello:setFrame( vec3( -2.5, 0., 2.5 ), vec3( 0., -1., 0. ), vec3( 0., 0., 1. ) )
	donatello:addPoint(5)

	donatello:setFrame( vec3( -.7, -1., 0. ), vec3( 1., 0., 0. ), vec3( 0., 0., 1. ) )
	donatello:addPoint(5)

	donatello:setFrame( vec3( 0., -1., 0. ), vec3( 1., 0., 0. ), vec3( 0., 0., 1. ) )
	donatello:endCrossSection()
	donatello:popState()

	donatello:setCarrierMode(0)
	donatello:setCrossSection(0)
	donatello:beginCylinder()
	donatello:move(4.)
	donatello:setCrossSection(0)
	donatello:addPoint(2)
	donatello:move(1.5)
	donatello:setCrossSection(1)
	donatello:addPoint(2)
	donatello:move(.5)
	donatello:setCrossSection(2)
	donatello:addPoint(2)
	donatello:move(.5)
	donatello:setCrossSection(3)
	donatello:endCylinder(2)

	m = donatello:getMesh()
	node = fg.meshnode(m)
	fgu:add(node)
end

local t = 0
function update(dt)
end

