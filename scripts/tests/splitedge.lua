--[[
	Splitting edges
--]]

module(...,package.seeall)

local m
local counter = 0

function setup()
	m = fg.mesh.primitives.icosahedron()
	fgu:addMesh(m)
end

function update(dt)	
	counter = counter + dt
	if (counter > 1 --[[ seconds --]] ) then
		-- split a random edge
		local v = m:selectRandomVertex()
		local f = v:getAdjacentFace()
		local pos = fg.pos(f,v)
		fg.splitEdge(m,pos)
		-- reset counter
		counter = 0
	end

end

