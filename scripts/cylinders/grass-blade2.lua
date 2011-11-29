
module(...,package.seeall)

m = {}
node = {}
function setup()
	bert = fg.turtle()

	m = bert:getMesh()
	node = fg.meshnode(m)
	fgu:add(node)


	m = bert:getMesh()
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt

    local age = t / 10
    age = smoothstep(0,1,age)

    bert = fg.turtle()
    makeLeaf(.01,20,1,1,bert)
	m = bert:getMesh()
	node:setMesh(m)
end

function makeLeaf(maxarch,maxlength,maxwidth,age,bert)
    local width = maxwidth * (0.1+smoothstep(.0,1.,age))--maxwidth * smoothstep(age,.0,1.)
    local length = maxlength * (smoothstep(.0,1.,age))--maxlength * smoothstep(age,.0,1.)
    local arch = maxarch * (smoothstep(.0,1.,age))--maxlength * smoothstep(age,.0,1.)
    local flat = smoothstep(0.,1.,age)
    local cs = {}
    local countdown = min(age*19,19)
    --print('\n')
    --print(age)
    --print(width)
    --print(length)
    bert:pushState()
    cs[1] = makeCrossSection(width,0*flat,bert)
    bert:popState()
    bert:pushState()
    cs[2] = makeCrossSection(width,1*flat,bert)
    bert:popState()

    bert:setCrossSection(cs[1])
    bert:setCarrierMode(0)
	bert:beginCylinder()
	bert:move(length)
    bert:setCrossSection(cs[2])
    bert:endCylinder(10)
end

function makeCrossSection2(don)
    local point
    local heading
    local up = vec3(0., 0., 1.)
    local theta
    local width = 1.0
    local flat = 0.

    point = vec3(-width*math.pi*11/12,-1.,0)*width
    heading = vec3(0,-1,0) 
    heading:normalise()

	don:setFrame(point,heading,up)
	don:setStiffness(.1,.1)
	don:beginCrossSection()

    point = vec3(-width*math.pi*.5,-1.1,0)*width
    heading = vec3(1,0,0) 
    heading:normalise()

	don:setFrame(point,heading,up)
	don:setStiffness(.4,.4)
	don:addPoint()

    point = vec3(0,-1.1,0)*width
    heading = vec3(1,0,0) 
    heading:normalise()

	don:setFrame(point,heading,up)
	don:setStiffness(.4,.4)
	don:addPoint()

    point = vec3(width*math.pi*.5,-1.1,0)*width
    heading = vec3(1,0,0) 
    heading:normalise()

	don:setFrame(point,heading,up)
	don:setStiffness(.4,.4)
	don:addPoint()

    point = vec3(width*math.pi*11/12,-1.,0)*width
    heading = vec3(0,1,0) 
    heading:normalise()

	don:setFrame(point,heading,up)
	don:setStiffness(.1,.1)
	don:addPoint()

    point = vec3(width*math.pi*.5,-.9,0)*width
    heading = vec3(-1,0,0) 
    heading:normalise()

	don:setFrame(point,heading,up)
	don:setStiffness(.4,.4)
	don:addPoint()

    point = vec3(0,-.9,0)*width
    heading = vec3(-1,0,0) 
    heading:normalise()

	don:setFrame(point,heading,up)
	don:setStiffness(.4,.4)
	don:addPoint()

    point = vec3(-width*math.pi*.5,-.9,0)*width
    heading = vec3(-1,0,0) 
    heading:normalise()

	don:setFrame(point,heading,up)
	don:setStiffness(.4,.4)
	don:addPoint()

    return don:endCrossSection()
end

function makeCrossSection(width,t,don)
    local point1,point2,point
    local heading1,heading2,heading
    local up = vec3(0., 0., 1.)
    local theta

    theta = math.pi*7./12.
    point1 = vec3(math.cos(theta),math.sin(theta),0)*width
    point2 = vec3((-3/2*math.pi+theta)*width,-1,0)
    point = lerp(point1,point2,t)

    heading1 = vec3(math.cos(theta),math.sin(theta),0)*width 
    heading1:normalise()
    heading2 = vec3(0,-1,0)
    heading = lerp(heading1,heading2,t)
    --print('\nPos')
    --print(point)
    --print('Heading')
    --print(heading)

	don:setFrame(point,heading,up)
	don:setStiffness(.4,.4)
	don:beginCrossSection()

    addPoint(math.pi,width,1,.4,1,don,t)
    addPoint(math.pi*3/2.,width,1,.4,1,don,t)
    addPoint(math.pi*2.,width,1,.6,1,don,t)

    addPoint2(math.pi*29/12,width,.4,1,don,t)

    addPoint(math.pi*2.,width,.8,.4,-1,don,t)
    addPoint(math.pi*3/2,width,.8,.4,-1,don,t)

    theta = math.pi
    point1 = vec3(math.cos(theta),math.sin(theta),0)*width*.8
    point2 = vec3((-3/2*math.pi+theta)*width,-.9,0)
    point = lerp(point1,point2,t)

    heading1 = vec3(-math.sin(theta),math.cos(theta),0)*-1
    heading2 = vec3(-1,0,0)
    heading = lerp(heading1,heading2,t)
    --print('\nPos')
    --print(point)
    --print('Heading')
    --print(heading)

    up = vec3(0,0,1)

	don:setStiffness(.4,.4)
	don:setFrame(point,heading,up)
    return don:endCrossSection()
end

function addPoint(theta,width,scale,s,dir,don,t)
    local point1 = vec3(math.cos(theta),math.sin(theta),0)*width
    local tmp = vec3(math.cos(theta),math.sin(theta),0)*width
    tmp:normalise()
    point1 = point1 - tmp*(1-scale)

    local point2 = vec3(width*(-3/2*math.pi+theta),-1-dir*.1,0)
    local point = lerp(point1,point2,t)

    local heading1 = vec3(-math.sin(theta),math.cos(theta),0)*dir
    local heading2 = vec3(dir,0,0)
    local heading = lerp(heading1,heading2,t)
    --print('\nPos')
    --print(point)
    --print('Heading')
    --print(heading)

    local up = vec3(0,0,1)

	don:setStiffness(s,s)
	don:setFrame(point,heading,up)
    don:addPoint()
end

function addPoint2(theta,width,s,dir,don,t)
    local point1 = vec3(math.cos(theta),math.sin(theta),0)*width
    local point2 = vec3(width*(11/12*math.pi),-1,0)
    local point = lerp(point1,point2,t)

    local heading1 = vec3(math.cos(theta),math.sin(theta),0)*width 
    heading1:normalise()
    heading1 = -heading1
    local heading2 = vec3(0,dir,0)
    local heading = lerp(heading1,heading2,t)
    --print('\nPos')
    --print(point)
    --print('Heading')
    --print(heading)

    local up = vec3(0,0,1)

	don:setStiffness(s,s)
	don:setFrame(point,heading,up)
    don:addPoint()
end
