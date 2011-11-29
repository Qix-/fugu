
module(...,package.seeall)

local m = {}
local n = 2
local depth = 7
local length = 1.5 
local width = 2.
local segs = 8
local points = 8
local ratio = .75
local ratio2 = .86
local angle = math.pi/5.5
local meander = .1
local noise = .25
function setup()
	bert = fg.turtle()
	m = fg.meshnode(bert:getMesh())
	fgu:add(m)

    bert:pitch(math.pi*-.48)
    bert:roll(math.pi*1)

	bert:setCarrierMode(1)

    bert:pushState()
    index = make_cs(points,noise,vec3(0,0,0),bert)
    bert:popState()

    bert:setCrossSection(1)
    bert:setScale(width)
	bert:beginCylinder()

    create_branch(depth,length,width,segs,points,ratio,angle,meander,noise,0,0,0,vec3(0,0,0),bert)

    bert:endCylinder(n)

    m:setMesh(bert:getMesh())
end

local t = 0
function update(dt)
	t = t + dt

end

function create_branch(depth,length,width,segs,points,ratio,angle,meander,noise,roll,rateCur,rateCur2,offset,bert)
    if depth < 1 then
        return
    end

    for i = 2, segs - 1, 1 do
	    bert:move(length)

		pos = bert:getPosition()
        pos = pos + offset
		rateCur = rateCur + meander * fracSum(pos.x, pos.y, pos.z, 1, 1)
		rateCur2 = rateCur2 + meander * fracSum(pos.x, pos.y, pos.z, 1, 1)
		bert:yaw(rateCur/2)
		bert:pitch(rateCur2/2)

        bert:pushState()
        index = make_cs(points,noise,vec3(0,0,0),bert)
        bert:popState()

        bert:setCrossSection(index)
        bert:addPoint(n)
		bert:yaw(rateCur/2)
		bert:pitch(rateCur2/2)
    end

	bert:move(length)

    bert:pushState()
    index = make_cs(points,noise,vec3(0,0,0),bert)
    bert:popState()

    bert:setCrossSection(index)
    bert:setScale(ratio*width)
	bert:addPoint(n)

    bert:pushState()
      if depth > 1 then 
        bert:setScale(width*ratio*.9)
    	bert:beginCylinder()
    
        bert:roll(roll)
        bert:pitch(angle)
        bert:roll(-roll)
        bert:move(length*segs*.2)
        bert:setScale(width*ratio*.7)
        bert:addPoint(n*segs*.4)
    
        create_branch(depth-1,length,(ratio-.1)*width,ratio2*segs,points,ratio,angle,meander,1.1*noise,roll+2.3999632297286,rateCur,rateCur2,offset+vec3(10,0,0),bert)
        bert:endCylinder(n)
      end
    bert:popState()


    create_branch(depth-1,length,ratio*width,ratio2*segs,points,ratio,angle,meander,1.1*noise,roll-2.3999632297286,rateCur,rateCur2,offset,bert)
end

function make_cs(npoints,noise,offset,bert)
    local pos = bert:getPosition()
    local heading = bert:getHeading()
    local up = bert:getUp()
    --print('getting up')
    --print(pos)
    --print(heading)
    --print(up)
    bert:setFrameRel( vec3( math.cos(2*math.pi), math.sin(2*math.pi), 0 ),
                      vec3( -math.sin(2*math.pi), math.cos(2*math.pi), 0 ),
                      vec3( 0, 0, 1 ) )
	local cpos
    cpos = bert:getPosition()
    --cpos = cpos + offset
    local dis = noise * fracSum(cpos.x, cpos.y, cpos.z, 1, 1)
    bert:setFrame(pos,heading,up)
    bert:setFrame( vec3( math.cos(2*math.pi), math.sin(2*math.pi), 0 ) * (1+dis),
                      vec3( -math.sin(2*math.pi), math.cos(2*math.pi), 0 ),
                      vec3( 0, 0, 1 ) )
    bert:beginCrossSection()
    bert:setFrame(pos,heading,up)

    for i = 1, npoints - 2, 1 do
        bert:setFrameRel( vec3( math.cos(2*i*math.pi/npoints), math.sin(2*i*math.pi/npoints), 0 ),
                          vec3( -math.sin(2*i*math.pi/npoints), math.cos(2*i*math.pi/npoints), 0 ),
                          vec3( 0, 0, 1 ) )
	    cpos = bert:getPosition()-- + offset
        --cpos = cpos + offset
        dis = noise * fracSum(cpos.x, cpos.y, cpos.z, 3, 1.5)
        bert:setFrame(pos,heading,up)
        bert:setFrame( vec3( math.cos(2*i*math.pi/npoints), math.sin(2*i*math.pi/npoints), 0 ) * (1+dis),
                          vec3( -math.sin(2*i*math.pi/npoints), math.cos(2*i*math.pi/npoints), 0 ),
                          vec3( 0, 0, 1 ) )
        bert:addPoint()
        bert:setFrame(pos,heading,up)
    end

    bert:setFrame( vec3( math.cos(2*math.pi), math.sin(2*math.pi), 0 ),
                          vec3( -math.sin(2*math.pi), math.cos(2*math.pi), 0 ),
                          vec3( 0, 0, 1 ) )
	cpos = bert:getPosition()-- + offset
    --cpos = cpos + offset
    dis = noise * fracSum(cpos.x, cpos.y, cpos.z, 3, 1.5)
    bert:setFrame(pos,heading,up)
    bert:setFrame( vec3( math.cos(2*math.pi*(1-1/npoints)), math.sin(2*math.pi*(1-1/npoints)), 0 ) * (1+dis),
                      vec3( -math.sin(2*math.pi*(1-1/npoints)), math.cos(2*math.pi*(1-1/npoints)), 0 ),
                      vec3( 0, 0, 1 ) )
    return bert:endCrossSection()
end

--[[
-- returns a normalised vector perpendicular to vec3 v
function perp(v)
    --print(v)
    local tmpv = cross(v,vec3(1,0,0))
    --print(tmpv)
	if (tmpv.x > 0.1 or tmpv.y > 0.1 or tmpv.z > 0.1) then --length(cross(v,vec3(1,0,0))) > 0.0001) then 
        print('1')
        return normalise(cross(v,vec3(1,0,0)))
	else   
        print('3')
        return normalise(cross(v,vec3(0,0,1))) 
    end
end
--]]

--[[	
 "Distributing many points on a sphere" by E.B. Saff and A.B.J. Kuijlaars,
  Mathematical Intelligencer 19.1 (1997) 5--11
--]]
--
--[[
for k=1 to N do
       h = -1 + 2*(k-1)/(N-1)
       theta[k] = arccos(h)
       if k=1 or k=N then phi[k] = 0
       else phi[k] = (phi[k-1] + 3.6/sqrt(N*(1-h^2))) mod (2*pi)
    endfor

  In Cartesian coordinates the required point on a sphere of radius 1 is
     (cos(theta)*sin(phi), sin(theta)*sin(phi), cos(phi))
--]]

--[[
function spherePoints(N)
	local points = {}
	local	theta = {}
	local phi = {}
	for k=1,N do
		local h = -1 + 2*(k-1)/(N-1)
		theta[k] = math.acos(h)
		if (k==1 or j==N) then 
			phi[k] = 0
		else 
			phi[k] = math.mod(phi[k-1] + 3.6/math.sqrt(N*(1-h*h)),2*math.pi)	
		end
		points[k] = vec3(math.cos(theta[k])*math.sin(phi[k]),math.sin(theta[k])*math.sin(phi[k]),math.cos(phi[k]))
        --print(points[k])
	end
	return points
end
--]]

-- returns a normalised vector perpendicular to vec3 v
function perp(v)
	if (v.x<v.y and v.x<v.z) then return normalise(cross(v,vec3(1,0,0)))
	elseif (v.y<v.x and v.y<v.z) then return normalise(cross(v,vec3(0,1,0)))
	else return normalise(cross(v,vec3(0,0,1))) end
end

--[[	
 "Distributing many points on a sphere" by E.B. Saff and A.B.J. Kuijlaars,
  Mathematical Intelligencer 19.1 (1997) 5--11
--]]
--
--[[
for k=1 to N do
       h = -1 + 2*(k-1)/(N-1)
       theta[k] = arccos(h)
       if k=1 or k=N then phi[k] = 0
       else phi[k] = (phi[k-1] + 3.6/sqrt(N*(1-h^2))) mod (2*pi)
    endfor

  In Cartesian coordinates the required point on a sphere of radius 1 is
     (cos(theta)*sin(phi), sin(theta)*sin(phi), cos(phi))
--]]

function spherePoints(N)
	local points = {}
	local	theta = {}
	local phi = {}
	for k=1,N do
		local h = -1 + 2*(k-1)/(N-1)
		theta[k] = math.acos(h)
		if (k==1 or j==N) then 
			phi[k] = 0
		else 
			phi[k] = math.mod(phi[k-1] + 3.6/math.sqrt(N*(1-h*h)),2*math.pi)	
		end
		points[k] = vec3(math.cos(theta[k])*math.sin(phi[k]),math.sin(theta[k])*math.sin(phi[k]),math.cos(phi[k]))
        print(points[k])
	end
	return points
end

function create_holdfast(phi,slen,ssegs,ln,donatello)
	for i = 1, nroots do
		donatello:roll(phi)
		donatello:pushState()
		local elipH = findEllipseH(A, B, SPA * i);
		donatello:pitch(math.pi)
		donatello:move(elipH)
		donatello:pitch(math.pi/4)
		donatello:move(A * math.sqrt(1 - ((elipH * elipH)/(B * B))))
		donatello:pitch(getAngleFromH(A, B, elipH))
		donatello:pushState()
        make_root(5,8,segs[i]*ssegs,.4,ln,width[i],length[i]*slen*(1+.5*elipH),vec3(0,0,0),vec3(0,0,0),bert)
		donatello:popState()
		donatello:popState()
	end
end
