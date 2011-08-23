module(...,package.seeall)

require "table"

-- Returns the loop of vertices surrounding v
function loopv(v)
	local result = {}
	local f = v:getAdjacentFace()
	local p = fg.pos(f,v)
	
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

-- Returns the loop of pos'es surrounding v
function loopp(v)
	local result = {}
	local f = v:getAdjacentFace()
	local p = fg.pos(f,v)
	
	local function push() -- push this pos onto the list		
		local newpos = fg.pos(p)
		table.insert(result,newpos)
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
