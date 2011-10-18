-- Help strings for most fg math functions
-- Also provides variants 
-- Overrides fgx.math

require 'core.util'

-- put the standard math library into the global namespace
require "math"
for k,v in pairs(math) do 
	_G[k]=v
	categorise(_G[k],"math") 
end

-- general maths functions
document[[min(a,b) = the lesser of a and b]](min) 
document[[lerp(a,b,t) interpolates between a and b]](lerp) 
document[[mix(a,b,t) mixes values a and b by amount t]](mix) 
document[[clamp(x,a,b) clamps x between a and b]](clamp) 
document[[slerp(a,b,t) spherically interpolates vec3 a and vec3 b]](slerp) 
document[[step(a,x) = 0 when x < a, 1 otherwise]](step)
document[[pulse(a,b,x) combines 2 steps: returns 1 if x is between a and b]](pulse)
document[[smoothstep(a,b,x) smooth interpolation using Horner's rule]](smoothstep)
document[[bias(a,x) 1D bias function]](bias)
document[[gain(g,x) gain control]](gain)
document[[gamma(g,x) standard gamma correct function]](gamma)
document[[inv_sqrt(x) inverse square root]](inv_sqrt)
document[[sqrt(x) = the square root of x]](sqrt)
document[[sqr(x) = the square of x]](sqr)
document[[sign(f) = the sign of f]](sign)
document[[noise(x), noise(x,y), noise(x,y,z) computes 1D, 2D or 3D Perlin noise]](noise)
document[[frac_sum(x,y,z,n,falloff) computes n octaves of summed fractal noise]](frac_sum)
document[[turbulence(x,y,z,n,falloff) computes n octaves of Perlin's turbulence function]](turbulence)
document[[random(low=0,high=1) returns a random number between low and high inclusive ]](random)
document[[gauss(mean,var) returns a gaussian distributed random number]](gauss)

-- put them in math category
foreach({min,lerp,mix,clamp,slerp,step,pulse,smoothstep,bias,gain,gamma,inv_sqrt,sqrt,sqr,sign,noise,frac_sum,turbulence,random,gauss}, function(_,f) categorise(f,"math") end)

-- phyllotaxis functions
document[[helper function for calculating phyllotaxis on oblate and prolate spheroids]](get_ellipse_area)
document[[helper function for calculating phyllotaxis on oblate and prolate spheroids]](find_ellipse_h)
document[[helper function for calculating phyllotaxis on oblate and prolate spheroids]](get_angle_from_h)

-- linear algebra
document[[vec3 represents a 3d vector. Example usage:
	creation: v = vec3(1.5,0,4)
	element access: v.x, v.y, v.z (or alternatively v.r, v.g, v.b)
	operators: v*2, v/3, v+w, v-w, -v]](vec3)

document[[length(v) computes the length of vec3 v]](length)
document[[distance(a,b) computes the distance between points (vec3) a and b]](distance)
document[[dot(a,b) computes the inner product of vec3 a and b]](dot)
document[[cross(a,b) computes the cross product of vec3 a and b]](cross)
document[[normalise(v) returns vec3 v normlised]](normalise)

foreach({vec3,length,distance,dot,cross,normalise}, function(_,f) categorise(f,"math") end)

document[[mat4 represents a 4d matrix. Example usage:
	creation: m = mat4()
	set to identity: m:set_basis(vec3(1,0,0),vec3(0,1,0),vec3(0,0,1))
	element access: m:get(row,col) row=0..3,col=0..3
	operators: m*2, m/3, m+n, m*n, m-n, -m
	make transformation matrix: 
		m:set_rotate_rad(angle,x,y,z)
		m:set_rotate_rad(angle,axis)
		m:set_translate(x,y,z)
		also see global functions R,Rv,T,S for shorthand]](mat4)
	
-- extra maths/geometric functions

function perp(v)
	if (v.x<v.y and v.x<v.z) then return normalise(cross(v,vec3(1,0,0)))
	elseif (v.y<v.x and v.y<v.z) then return normalise(cross(v,vec3(0,1,0)))
	else return normalise(cross(v,vec3(0,0,1))) end
end
document[[perp(v:vec3) returns a normalised vector perpendicular to vec3 v]](perp)

function distribute_points_sphere(N)
	local points = {}
	local	theta = {}
	local phi = {}
	for k=1,N do
		local h = -1 + 2*(k-1)/(N-1)
		theta[k] = math.acos(h)
		if (k==1 or k==N) then 
			phi[k] = 0
		else 
			phi[k] = math.mod(phi[k-1] + 3.6/math.sqrt(N*(1-h*h)),2*math.pi)	
		end
		points[k] = vec3(math.cos(theta[k])*math.sin(phi[k]),math.sin(theta[k])*math.sin(phi[k]),math.cos(phi[k]))
	end
	return points
end
document[[
  distribute_points_sphere(n) returns n equally distributed points on a sphere.

 "Distributing many points on a sphere" by E.B. Saff and A.B.J. Kuijlaars,
  Mathematical Intelligencer 19.1 (1997) 5--11
]](distribute_points_sphere)

function project(a,b)
	local bn = normalise(b)
	local ab = dot(a,b)/length(b)
	return bn*ab
end
document[[project(a,b) projects vec3 a onto vec3 b]](project)

function centroid(poly)
	local cx = 0
	local cy = 0 
	local signedArea = 0
	for i=1,#poly do	
		local ip1 = ((i==#poly) and 1) or i+1
		local pi = poly[i]
		local pip = poly[ip1]
		local common = (pi.x*pip.y - pip.x*pi.y)
		cx = cx + (pi.x + pip.x)*common
		cy = cy + (pi.y + pip.y)*common
		signedArea = signedArea + common		 
	end	
	signedArea = signedArea / 2
	cx = cx / (6*signedArea)
	cy = cy / (6*signedArea)
	return vec3(cx,cy,0)			
end
document[[centroid(poly) computes the centroid of a 2d polygon defined by a list of vec3 (with z = 0)]](centroid)

function acentroidvl(vl,n)
	-- new basis
	local x = perp(n)
	local y = cross(n,x)
	local z = n
	local mtx = mat4():setBasis(x,y,z)
	
	-- calculate projected points
	local proj = {}
	local zAvg = 0
	for i=1,#vl do
		local p = vl[i].p
		local pp = p * mtx
		proj[#proj+1] = pp		
		zAvg = zAvg + pp.z
	end
	zAvg = zAvg / #vl
	local guess = centroid(proj)
	
	return x*guess.x + y*guess.y + z*zAvg
end
document[[acentroidvl(vl,n) computes the approximate centroid of a list of 
vertices defining a closed polygon (not necessarily planar).
The normalised vector, n, defines the plane on which the polygon approximately lies]](acentroidvl)
 