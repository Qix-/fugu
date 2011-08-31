--[[
-- Utilities for manipulating Pos objects
--]]

module(...,package.seeall)

require 'table'

local faces_iter -- defined below

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