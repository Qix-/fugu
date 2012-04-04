module(...,package.seeall)

local n = nil
local s = 0

function setup()
	local m = tetrahedron()
	n = meshnode(m)
	fgu:add(n)
end

local tick = 0
function update(dt)
	tick = tick+dt
	if (tick > 2) then
		s = s+1
		local m = tetrahedron()
		if s>0 then	m:smooth_subdivide(s) end
		n:set_mesh(m)	
		tick = 0
	end
end

