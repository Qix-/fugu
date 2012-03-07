module(...,package.seeall)

local points

local function floob(x,y,z)
	local m = huge
	local v = vec3(x,y,z)	
	for i=1,#points do
		local d = segment_point_dist(points[i],vec3(0,0,0),v)
		if (d < m) then m=d end
	end
	local sqd = sqr(x)+sqr(y)+sqr(z)
	return m - .1*(2-sqd)
end

local res = 24
local m, n
function setup()
	points = distribute_points_sphere(16)
	for i=1,#points do
		points[i] = points[i]*.7
	end
	m = iso(res, floob)	
	n = meshnode(m)
	fgu:add(n)
end

function update(dt)	
	--[[
	for i=1,#points do
		points[i] = points[i]+random_vec3(-.5,.5)*(.1*sin(fgu.t*.1))
	end
	m = iso(res,floob)	
	n:set_mesh(m)	
	--]]
end