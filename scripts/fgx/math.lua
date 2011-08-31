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