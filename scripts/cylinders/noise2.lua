
module(...,package.seeall)

local m = {}
local length = 2.225
local width = 1.5
local segs = 8
local ncps = 16
local noise = .4
function setup()
	luke = fg.turtle()

    make_root(5,ncps,segs,noise,0.,width,length,vec3(0,0,0),vec3(0,0,0),luke)

	m = fg.meshnode(luke:getMesh())
	fgu:add(m)
end

local t = 0
function update(dt)
	t = t + dt
end

function make_root(n,npcs,ncs,noise,noise2,width,l,offset1,offset2,luke)
    local index

	luke:setCarrierMode(1)

    luke:pushState()
    index = make_cs(npcs,noise,offset,luke)
    luke:popState()

    luke:setCrossSection(1)
    luke:setScale(width)
	luke:beginCylinder()

    local rateCur = 0.0
    local rateCur2 = 0.00
    for i = 2, ncs - 1, 1 do
	    luke:move(l)

		pos = luke:getPosition() + offset2
		rateCur = rateCur + noise2 * frac_sum(pos.x, pos.y, pos.z, 1, 1)
		--rateCur2 = rateCur2 + noise2 * fracSum(pos.x, pos.y, pos.z, 1, 1)
		luke:yaw(rateCur/2)
		luke:pitch(rateCur2/2)

        luke:pushState()
        index = make_cs(npcs,noise,0,luke)
        luke:popState()

        luke:setCrossSection(index)
        luke:addPoint(n)
		luke:yaw(rateCur/2)
		luke:pitch(rateCur2/2)
    end

	luke:move(l)

    luke:pushState()
    index = make_cs(npcs,noise,0,luke)
    luke:popState()

    luke:setCrossSection(index)
    --luke:setScale(.01*width)
    luke:endCylinder(n)
end

function make_cs(npoints,noise,offset,luke)
    local pos = luke:getPosition()
    local heading = luke:getHeading()
    local up = luke:getUp()
    --print('getting up')
    --print(pos)
    --print(heading)
    --print(up)
    luke:setFrameRel( vec3( math.cos(2*math.pi), math.sin(2*math.pi), 0 ),
                      vec3( -math.sin(2*math.pi), math.cos(2*math.pi), 0 ),
                      vec3( 0, 0, 1 ) )
	local cpos
    cpos = luke:getPosition()
    --cpos = cpos + offset
    local dis = noise * frac_sum(cpos.x, cpos.y, cpos.z, 1, 1)
    luke:setFrame(pos,heading,up)
    luke:setFrame( vec3( math.cos(2*math.pi), math.sin(2*math.pi), 0 ) * (1+dis),
                      vec3( -math.sin(2*math.pi), math.cos(2*math.pi), 0 ),
                      vec3( 0, 0, 1 ) )
    luke:beginCrossSection()
    luke:setFrame(pos,heading,up)

    for i = 1, npoints - 2, 1 do
        luke:setFrameRel( vec3( math.cos(2*i*math.pi/npoints), math.sin(2*i*math.pi/npoints), 0 ),
                          vec3( -math.sin(2*i*math.pi/npoints), math.cos(2*i*math.pi/npoints), 0 ),
                          vec3( 0, 0, 1 ) )
	    cpos = luke:getPosition()-- + offset
        --cpos = cpos + offset
        dis = noise * frac_sum(cpos.x, cpos.y, cpos.z, 3, 1.5)
        luke:setFrame(pos,heading,up)
        luke:setFrame( vec3( math.cos(2*i*math.pi/npoints), math.sin(2*i*math.pi/npoints), 0 ) * (1+dis),
                          vec3( -math.sin(2*i*math.pi/npoints), math.cos(2*i*math.pi/npoints), 0 ),
                          vec3( 0, 0, 1 ) )
        luke:addPoint()
        luke:setFrame(pos,heading,up)
    end

    luke:setFrame( vec3( math.cos(2*math.pi), math.sin(2*math.pi), 0 ),
                          vec3( -math.sin(2*math.pi), math.cos(2*math.pi), 0 ),
                          vec3( 0, 0, 1 ) )
	cpos = luke:getPosition()-- + offset
    --cpos = cpos + offset
    dis = noise * frac_sum(cpos.x, cpos.y, cpos.z, 3, 1.5)
    luke:setFrame(pos,heading,up)
    luke:setFrame( vec3( math.cos(2*math.pi*(1-1/npoints)), math.sin(2*math.pi*(1-1/npoints)), 0 ) * (1+dis),
                      vec3( -math.sin(2*math.pi*(1-1/npoints)), math.cos(2*math.pi*(1-1/npoints)), 0 ),
                      vec3( 0, 0, 1 ) )
    return luke:endCrossSection()
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
        make_root(5,8,segs[i]*ssegs,.4,ln,width[i],length[i]*slen*(1+.5*elipH),vec3(0,0,0),vec3(0,0,0),luke)
		donatello:popState()
		donatello:popState()
	end
end
