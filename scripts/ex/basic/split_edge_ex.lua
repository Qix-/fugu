--[[
	Demonstrates split_edge()
--]]

module(...,package.seeall)

local m
local counter = 0

function setup()
	m = icosahedron()
	fgu:add(meshnode(m))
end

function update(dt)	
	counter = counter + dt
	if (counter > 1 --[[ seconds --]] ) then
		-- split a random edge
		local v = m:select_random_vertex()
		local f = v:get_adjacent_face()
		local pos = fg.pos(f,v)
		split_edge(m,pos)
		-- reset counter
		counter = 0
	end

end

