--[[
	Test the ui routines
	BP 31/10/2011
--]]
module(...,package.seeall)

local m -- mesh
local verts = {} -- vertices
local opp = {} -- original points polar

local sf -- superformula function (defined below)

-- superformula parameters
local s1 = {a=1,b=1,m=7,n1=0.2,n2=1.7,n3=1.7}
local s2 = {a=1,b=1,m=7,n1=0.2,n2=1.7,n3=1.7}
-- global instances of superformulae
s1c = nil
s2c = nil
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
	
	add_slider {var="s1c.a", value=s1.a, low=0, high=5}
	add_slider {var="s1c.b", value=s1.b, low=0, high=5}
	add_slider {var="s1c.m", value=s1.m, low=0, high=14}
	add_slider {var="s1c.n1", value=s1.n1, low=0, high=2}
	add_slider {var="s1c.n2", value=s1.n2, low=0, high=2}
	add_slider {var="s1c.n3", value=s1.n3, low=0, high=2}
end

function update(dt)
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