module(...,package.seeall)

require "table"

-- Return an array of vertices surrounding v
function onering(v)
	local result = {}
	local f = v:getAdjacentFace()
	local p = fg.pos(f,0,v)
	
	local function push() -- add a new outside vertex to the list
		p:flipV()
		table.insert(result,p.v)
		p:flipV()
	end	
	push()
		
	p:flipE()
	p:flipF()
	while (p.f~=f) do
		push()
		p:flipE()
		p:flipF()
	end
		
	return result
end
