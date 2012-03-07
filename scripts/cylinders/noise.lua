
module(...,package.seeall)

local A = 9	-- sphere radius
local B = 1    	-- prolate
local elemR = .5 	-- element radius
local N = 50		-- number of elements
local PHI = 2.39982772	-- golden ratio
local SPA = .5*math.pi*math.pi 	-- segment area

local m = {}
local nroots = 50
local length = {}
local width = {}
local segs = {}
local points = {}
function setup()
    local meanlength = 1.5
    local meanwidth = 1
    local meansegs = 10

    for i = 1, nroots, 1 do
		length[i] = randomN(meanlength,.1)
		width[i] = randomN(meanwidth,.1)
        segs[i] = randomN(meansegs,.8)
    end
--    local n = 5
--    local npcs = 8
--    local noise = .5
--    local ncs = 10 
--    local l = 1
--    local width = 1
--
--    local noise2 = .5
--    local offset = vec3(0,0,0)
--    local offset2 = vec3(0,0,0)
--
--	bert = fg.turtle()
--
----    for i = 1, ncs, 1 do
----        bert:pushState()
----        make_cs(npcs,noise,0,bert)
----        bert:popState()
----    end
--
--	bert:setCarrierMode(0)
--
--    bert:pushState()
--    make_cs(npcs,noise,offset,bert)
--    bert:popState()
--
--    bert:setCrossSection(1)
--    bert:setScale(width)
--	bert:beginCylinder()
--
--    local rateCur = 0.04
--    local pos
--    for i = 2, ncs - 1, 1 do
--	    bert:move(l)
--
--		pos = bert:getPosition()-- + offset2
--        pos = pos + offset2
--		rateCur = rateCur + noise2 * fracSum(pos.x, pos.y, pos.z, 1, 1)
--		bert:yaw(rateCur/2)
--
--        bert:pushState()
--        make_cs(npcs,noise,0,bert)
--        bert:popState()
--
--        bert:setCrossSection(i)
--        bert:addPoint(n)
--		bert:yaw(rateCur/2)
--    end
--
--	bert:move(l)
--
--    bert:pushState()
--    make_cs(npcs,noise,0,bert)
--    bert:popState()
--
--    bert:setCrossSection(ncs)
--    bert:setScale(.1*width)
--    bert:endCylinder(n)

	bert = fg.turtle()
	m = fg.meshnode(bert:getMesh())
	fgu:add(m)
end

local t = 0
function update(dt)
	t = t + dt

	bert = fg.turtle()

    local ln = .2
    local ssegs = 1
    local slen = 1
    if t < 5 then
        ssegs = 1--smoothstep(0, 5, t)
        slen = smoothstep(0, 5, t)
    else
        ssegs = 1
        slen = 1
    end
    if t < 2 then
        ln = smoothstep(0, 2, t) / 5
    elseif t < 4 then
        ln = .2 - smoothstep(2, 4, t) / 5
    elseif t < 6 then
        ln = smoothstep(4, 6, t) / 5
    elseif t < 8 then
        ln = .2 - smoothstep(6, 8, t) / 5
    end

--	for i = 1, nroots do
--		-- create a root at points[i]
--
--        --print('setting up')
--        --print(p)
--        --print(dir)
--        --print(up)
--        --print()
--		bert:pushState()
--			bert:setFrame(p*.3,dir,up)
--            make_root(5,8,segs[i],.5,ln,width[i],length[i],vec3(0,0,0),vec3(0,0,0),bert)
--		bert:popState()
--	end
    
    create_holdfast(PHI,slen,ssegs,ln,bert)
    m:setMesh(bert:getMesh())
--    local n = 5
--    local npcs = 8
--    local noise = .5
--    local ncs = 10 
--    local l = 1
--    local width = 1
--
--    local noise2 = .5
--    local offset = vec3(0,0,0)
--    local offset2 = vec3(0,0,0)
end

function make_root(n,npcs,ncs,noise,noise2,width,l,offset1,offset2,bert)
    local index

	bert:setCarrierMode(1)

    bert:pushState()
    index = make_cs(npcs,noise,offset,bert)
    bert:popState()

    bert:setCrossSection(1)
    bert:setScale(width)
	bert:beginCylinder()

    local rateCur = 0.0
    local rateCur2 = 0.05
    for i = 2, ncs - 1, 1 do
	    bert:move(l)

		pos = bert:getPosition() + offset2
		rateCur = rateCur + noise2 * fracSum(pos.x, pos.y, pos.z, 1, 1)
		--rateCur2 = rateCur2 + noise2 * fracSum(pos.x, pos.y, pos.z, 1, 1)
		bert:yaw(rateCur/2)
		bert:pitch(rateCur2/2)

        bert:pushState()
        index = make_cs(npcs,noise,0,bert)
        bert:popState()

        bert:setCrossSection(index)
        bert:addPoint(n)
		bert:yaw(rateCur/2)
		bert:pitch(rateCur2/2)
    end

	bert:move(l)

    bert:pushState()
    index = make_cs(npcs,noise,0,bert)
    bert:popState()

    bert:setCrossSection(index)
    bert:setScale(.01*width)
    bert:endCylinder(n)
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
