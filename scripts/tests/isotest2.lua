module(...,package.seeall)

local points

local function floob(x,y,z)
	local m = {huge,huge}
	local v = vec3(x,y,z)	
	foreach(points, function(_,p)
		local d = sqr(p.x-v.x) + sqr(p.y-v.y) + sqr(p.z-v.z)
		if (d < m[1]) then
			m[2] = m[1]
			m[1] = d
		elseif (d < m[2]) then 
			m[2] = d
		end
	end)
	if m[1]==huge then m[1] = 0 end
	if m[2]==huge then m[2] = 0 end
	return .6*m[1] + .3*m[2] - .1
end

local res = 30
local m, n
function setup()
	points = distribute_points_sphere(30)
	for i=1,#points do
		points[i] = points[i]*.5
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