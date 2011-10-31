--[[
	Johan Gielis' superformula.
	Extended to 3D, from: http://paulbourke.net/geometry/supershape3d/
	BP 26/10/2011
--]]
module(...,package.seeall)

local m -- mesh
local verts = {} -- vertices
local opp = {} -- original points polar

local sf -- superformula function (defined below)

-- superformula parameters
local s1 = {a=1,b=1,m=7,n1=0.2,n2=1.7,n3=1.7}
local s2 = {a=1,b=1,m=7,n1=0.2,n2=1.7,n3=1.7}
s1c = nil
s2c = nil -- copies of superformulas for animation
local wave, awave -- wavy functions with params (period,offset)

function setup()
	m = icosahedron()
	m:subdivide(3)
	verts = vertexlist(m)
	for i,v in ipairs(verts) do
		local r,th,ph = polar(v.p.x,v.p.y,v.p.z)
		ph = ph - pi/2 -- fix the range of ph 
		opp[i] = {r,th,ph}
		v.p = sf(s1,s2,-th,ph) -- negating theta helps..
		v.c = vec3(awave(1.2,ph),awave(1,th),awave(1,0))
	end
	m:sync()
	fgu:add(meshnode(m))
	
	s1c = deepcopy(s1)
	s2c = deepcopy(s2)
end

function update(dt)
	s1c.m = awave(1,0)*s1.m
	s1c.n1 = 1 + .5*awave(2,pi/4)*s1.n1
	s1c.n2 = 2 + 1.8*awave(2.4,pi/8)*s1.n2	
	s1c.n3 = 1 + 1.8*awave(2.5,pi/8)*s1.n3	
	
	s2c.m = awave(1.3,.2)*s2.m
	s2c.n1 = 1 + .5*awave(2,pi/4)*s2.n1
	s2c.n2 = 2 + 1.8*awave(2.4,pi/8)*s2.n2	
	s2c.n3 = 1 + 1.8*awave(2.5,pi/8)*s2.n3	

	for i,v in ipairs(verts) do		
		local r,th,ph = opp[i][1],opp[i][2],opp[i][3]		
		v.p = sf(s1c,s2,-th,ph) -- negating theta helps..
		v.c = vec3(awave(1.2,ph),awave(1,th),awave(1,0))
	end
end

local function sf_r(p,ang)
	local term = p.m*ang/4	
	return pow(
		pow(abs((1/p.a) * cos(term)),p.n2) + 
		pow(abs((1/p.b) * sin(term)),p.n3),
		-1/p.n1)		
end

-- params: shape 1, shape 2, theta, phi
sf = function(s1,s2,th,ph) 
	local cosphi = cos(ph)
	local r1 = sf_r(s1,th)
	local r2 = sf_r(s2,ph)
	local x = r1*cos(th)*r2*cosphi
	local y = r1*sin(th)*r2*cosphi	
	local z = r2*sin(ph)
	return vec3(x,y,z)
end

wave = function(p,o) 
		return sin(p*fgu.t+o) 
	end
awave = function(p,o) 
	return .5+.5*sin(p*fgu.t+o) 
end