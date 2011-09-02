--[[
-- Utilities for manipulating Pos objects
--]]

module(...,package.seeall)

require 'table'

local faces_iter -- defined below

-- return the outermost vertices of a cap (a fan of pos'es)
function capov(cap)
	local verts = {}
	for _,p in ipairs(cap) do
		local pp = fg.pos(p)
		pp:flipV()
		verts[#verts+1] = pp.v
	end
	return verts
end



-- get an iterator over all the faces in this poslist 
function ifaces(poslist)
	local state = {list = poslist, last = poslist[1], index = 1}	
	return faces_iter, state
end

function faces_iter (state)
	if (state.index==1) then 
		state.index = state.index + 1
		return (state.index-1), state.last.f
	else
		local next = state.list[state.index]
		if (next==nil) then 
			return nil, nil
		else
			while (next==state.last) do			
				state.index = state.index + 1
				next = state.list[state.index]
			end
			state.last = next
			state.index = state.index + 1		
			return (state.index-1), next.f
		end
	end	 
end