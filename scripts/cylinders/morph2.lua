
module(...,package.seeall)

local m = {}
function setup()
	bert = fg.turtle()
    bert:pushState()

    -- Create a cross section
    bert:setFrame(vec3(1.3,.0,0.),vec3(0.,1.,0.),vec3(0.,0.,1.))
	bert:setStiffness(0.,0.)
	bert:beginCrossSection()

    bert:setFrame(vec3(.0,1.3,0.),vec3(0.,1.,0.),vec3(0.,0.,1.))
	bert:addPoint()

    bert:setFrame(vec3(-1.3,.0,0.),vec3(0.,1.,0.),vec3(0.,0.,1.))
	bert:endCrossSection()

    bert:setStiffness(.6,.6)

    local pos, heading, up, t

    up = vec3( 0, 0, 1 )
    t = 0
    pos = vec3( math.cos( 0.*math.pi/6 ), math.sin( 0.*math.pi/6), 0. ) * 2.
    head = vec3( -math.pi/6*math.sin( t*math.pi/6 ), math.pi/6*math.cos(t*math.pi/6), 0. )
    head:normalise()
    bert:setFrame( pos, head, up )
    bert:beginCrossSection()

    t = 1
    pos = vec3( math.cos( 1.*math.pi/6 ), math.sin( 1.*math.pi/6), 0. ) * .5
    head = vec3( -math.pi/6*math.sin( t*math.pi/6 ), math.pi/6*math.cos(t*math.pi/6), 0. )
    head:normalise()
    bert:setFrame( pos, head, up )
    bert:addPoint()

    t = 2
    pos = vec3( math.cos( 2.*math.pi/6 ), math.sin( 2.*math.pi/6), 0. ) * 2.
    head = vec3( -math.pi/6*math.sin( t*math.pi/6 ), math.pi/6*math.cos(t*math.pi/6), 0. )
    head:normalise()
    bert:setFrame( pos, head, up )
    bert:addPoint()

    t = 3
    pos = vec3( math.cos( 3.*math.pi/6 ), math.sin( 3.*math.pi/6), 0. ) * .5
    head = vec3( -math.pi/6*math.sin( t*math.pi/6 ), math.pi/6*math.cos(t*math.pi/6), 0. )
    head:normalise()
    bert:setFrame( pos, head, up )
    bert:addPoint()

    t = 4
    pos = vec3( math.cos( 4.*math.pi/6 ), math.sin( 4.*math.pi/6), 0. ) * 2
    head = vec3( -math.pi/6*math.sin( t*math.pi/6 ), math.pi/6*math.cos(t*math.pi/6), 0. )
    head:normalise()
    bert:setFrame( pos, head, up )
    bert:addPoint()

    t = 5
    pos = vec3( math.cos( 5.*math.pi/6 ), math.sin( 5.*math.pi/6), 0. ) * .5
    head = vec3( -math.pi/6*math.sin( t*math.pi/6 ), math.pi/6*math.cos(t*math.pi/6), 0. )
    head:normalise()
    bert:setFrame( pos, head, up )
    bert:addPoint()

    t = 6
    pos = vec3( math.cos( 6.*math.pi/6 ), math.sin( 6.*math.pi/6), 0. ) * 2 
    head = vec3( -math.pi/6*math.sin( t*math.pi/6 ), math.pi/6*math.cos(t*math.pi/6), 0. )
    head:normalise()
    bert:setFrame( pos, head, up )
    bert:addPoint()

    t = 7
    pos = vec3( math.cos( 7.*math.pi/6 ), math.sin( 7.*math.pi/6), 0. ) * .5
    head = vec3( -math.pi/6*math.sin( t*math.pi/6 ), math.pi/6*math.cos(t*math.pi/6), 0. )
    head:normalise()
    bert:setFrame( pos, head, up )
    bert:addPoint()

    t = 8
    pos = vec3( math.cos( 8.*math.pi/6 ), math.sin( 8.*math.pi/6), 0. ) * 2 
    head = vec3( -math.pi/6*math.sin( t*math.pi/6 ), math.pi/6*math.cos(t*math.pi/6), 0. )
    head:normalise()
    bert:setFrame( pos, head, up )
    bert:addPoint()

    t = 9
    pos = vec3( math.cos( 9.*math.pi/6 ), math.sin( 9.*math.pi/6), 0. ) * .5
    head = vec3( -math.pi/6*math.sin( t*math.pi/6 ), math.pi/6*math.cos(t*math.pi/6), 0. )
    head:normalise()
    bert:setFrame( pos, head, up )
    bert:addPoint()

    t = 10 
    pos = vec3( math.cos( 10.*math.pi/6 ), math.sin( 10.*math.pi/6), 0. ) * 2
    head = vec3( -math.pi/6*math.sin( t*math.pi/6 ), math.pi/6*math.cos(t*math.pi/6), 0. )
    head:normalise()
    bert:setFrame( pos, head, up )
    bert:addPoint()

    t = 11
    pos = vec3( math.cos( 11.*math.pi/6 ), math.sin( 11.*math.pi/6), 0. ) * .5
    head = vec3( -math.pi/6*math.sin( t*math.pi/6 ), math.pi/6*math.cos(t*math.pi/6), 0. )
    head:normalise()
    bert:setFrame( pos, head, up )
    bert:endCrossSection()
--	arr4.push_back( Vec3( cos( 0.*M_PI/6 ), sin( 0.*M_PI/6), 0. ) * .2 );
--	arr4.push_back( Vec3( cos( 1.*M_PI/6 ), sin( 1.*M_PI/6), 0. ) * .05 );
--
--	arr4.push_back( Vec3( cos( 2.*M_PI/6 ), sin( 2.*M_PI/6), 0. ) * .2 );
--	arr4.push_back( Vec3( cos( 3.*M_PI/6 ), sin( 3.*M_PI/6), 0. ) * .05 );
--
--	arr4.push_back( Vec3( cos( 4.*M_PI/6 ), sin( 4.*M_PI/6), 0. ) * .2 );
--	arr4.push_back( Vec3( cos( 5.*M_PI/6 ), sin( 5.*M_PI/6), 0. ) * .05 );
--
--	arr4.push_back( Vec3( cos( 6.*M_PI/6 ), sin( 6.*M_PI/6), 0. ) * .2 );
--	arr4.push_back( Vec3( cos( 7.*M_PI/6 ), sin( 7.*M_PI/6), 0. ) * .05 );
--
--	arr4.push_back( Vec3( cos( 8.*M_PI/6 ), sin( 8.*M_PI/6), 0. ) * .2 );
--	arr4.push_back( Vec3( cos( 9.*M_PI/6 ), sin( 9.*M_PI/6), 0. ) * .05 );
--
--	arr4.push_back( Vec3( cos( 10.*M_PI/6 ), sin( 10.*M_PI/6), 0. ) * .2 );
--	arr4.push_back( Vec3( cos( 11.*M_PI/6 ), sin( 11.*M_PI/6), 0. ) * .05 );
--
--	t = 0.;
--	gradT = Vec3( -M_PI/6*sin( t*M_PI/6 ), M_PI/6*cos(t*M_PI/6), 0. ) * .15;
--	grad3.push_back( std::pair<Vec3,Vec3> ( gradT,gradT ) );
--	t = 1.;
--	gradT = Vec3( -M_PI/6*sin( t*M_PI/6 ), M_PI/6*cos(t*M_PI/6), 0. ) * .05;
--	grad3.push_back( std::pair<Vec3,Vec3> ( gradT,gradT ) );
--
--	t = 2.;
--	gradT = Vec3( -M_PI/6*sin( t*M_PI/6 ), M_PI/6*cos(t*M_PI/6), 0. ) * .15;
--	grad3.push_back( std::pair<Vec3,Vec3> ( gradT,gradT ) );
--	t = 3.;
--	gradT = Vec3( -M_PI/6*sin( t*M_PI/6 ), M_PI/6*cos(t*M_PI/6), 0. ) * .05;
--	grad3.push_back( std::pair<Vec3,Vec3> ( gradT,gradT ) );
--
--	t = 4.;
--	gradT = Vec3( -M_PI/6*sin( t*M_PI/6 ), M_PI/6*cos(t*M_PI/6), 0. ) * .15;
--	grad3.push_back( std::pair<Vec3,Vec3> ( gradT,gradT ) );
--	t = 5.;
--	gradT = Vec3( -M_PI/6*sin( t*M_PI/6 ), M_PI/6*cos(t*M_PI/6), 0. ) * .05;
--	grad3.push_back( std::pair<Vec3,Vec3> ( gradT,gradT ) );
--
--	t = 6.;
--	gradT = Vec3( -M_PI/6*sin( t*M_PI/6 ), M_PI/6*cos(t*M_PI/6), 0. ) * .15;
--	grad3.push_back( std::pair<Vec3,Vec3> ( gradT,gradT ) );
--	t = 7.;
--	gradT = Vec3( -M_PI/6*sin( t*M_PI/6 ), M_PI/6*cos(t*M_PI/6), 0. ) * .05;
--	grad3.push_back( std::pair<Vec3,Vec3> ( gradT,gradT ) );
--
--	t = 8.;
--	gradT = Vec3( -M_PI/6*sin( t*M_PI/6 ), M_PI/6*cos(t*M_PI/6), 0. ) * .15;
--	grad3.push_back( std::pair<Vec3,Vec3> ( gradT,gradT ) );
--	t = 9.;
--	gradT = Vec3( -M_PI/6*sin( t*M_PI/6 ), M_PI/6*cos(t*M_PI/6), 0. ) * .05;
--	grad3.push_back( std::pair<Vec3,Vec3> ( gradT,gradT ) );
--
--	t = 10.;
--	gradT = Vec3( -M_PI/6*sin( t*M_PI/6 ), M_PI/6*cos(t*M_PI/6), 0. ) * .15;
--	grad3.push_back( std::pair<Vec3,Vec3> ( gradT,gradT ) );
--	t = 11.;
--	gradT = Vec3( -M_PI/6*sin( t*M_PI/6 ), M_PI/6*cos(t*M_PI/6), 0. ) * .05;
--	grad3.push_back( std::pair<Vec3,Vec3> ( gradT,gradT ) );


    bert:popState()
	bert:setCarrierMode(1)
	bert:setCrossSection(1)
	bert:beginCylinder()
	bert:move(10.)
	bert:setCrossSection(2)
    bert:endCylinder(30)

	m = bert:getMesh()
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt
end


