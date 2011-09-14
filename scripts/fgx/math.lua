--[[
-- Various maths bits and pieces
--]] 

module(...,package.seeall)

-- returns a normalised vector perpendicular to vec3 v
function perp(v)
	if (v.x<v.y and v.x<v.z) then return normalise(cross(v,vec3(1,0,0)))
	elseif (v.y<v.x and v.y<v.z) then return normalise(cross(v,vec3(0,1,0)))
	else return normalise(cross(v,vec3(0,0,1))) end
end

--[[
  Returns N equally distributed points on a sphere.

 "Distributing many points on a sphere" by E.B. Saff and A.B.J. Kuijlaars,
  Mathematical Intelligencer 19.1 (1997) 5--11
--]]
function spherePoints(N)
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

-- project vec3 a onto vec3 b
function project(a,b)
	local bn = normalise(b)
	local ab = dot(a,b)/length(b)
	return bn*ab
end

--[[
  Compute the centroid of a 2d polygon defined by the poly (list of vec3, where z = 0)
--]]
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

--[[
  Compute the approximate centroid of a list of vertices defining a closed
  polygon (not necessarily planar)
 
  The normalised vector, n, defines the plane on which 
  the polygon approximately lies
--]]
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