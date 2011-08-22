--[[
-- Various spatial transformations
--]]

module(...,package.seeall)

require 'fgx.pos'

-- scales a list of faces around a specified point
-- amount = 
--   0 -> all vertices will be moved to the center point
--   1 -> all vertices will remain in place
--   (0,1) -> faces will shrink
--   >1 -> faces will grow 

function scalef(poslist,center,amount)
	-- first we build a list of all unique vertices
	-- so no vertex gets scaled more than once
	local unique = {}
	
	local function addUnique(v)
		for _,u in ipairs(unique) do 
			if u==v then return end
		end
		table.insert(unique,v) 
	end
	
	for _,f in fgx.pos.ifaces(poslist) do
		for i=1,3 do
			addUnique(f:v(i-1))
		end		
	end
	
	-- now we can scale the vertices
	for _,u in ipairs(unique) do
		u.p = center + (u.p - center)*amount
	end 
end