require "table"
require "core.pos"

-- Returns the loop of vertices surrounding v
function loopv(v)
	local result = {}
	local f = v:get_adjacent_face()
	local p = pos(f,v)
	
	local function push() -- add a new outside vertex to the list
		p:flip_v()
		table.insert(result,p.v)
		p:flip_v()
	end	
	push()
		
	p:flip_e()
	p:flip_f()
	while (p.f~=f) do
		push()
		p:flip_e()
		p:flip_f()
	end
		
	return result
end
document[[loopv(v:vertex) returns the loop of vertices surrounding v]](loopv)
categorise(loopv,"mesh")

-- Returns the loop of pos'es surrounding v
function loopp(v)
	local result = {}
	local f = v:get_adjacent_face()
	local p = pos(f,v)
	
	local function push() -- push this pos onto the list		
		local newpos = pos(p)
		table.insert(result,newpos)
	end	
	push()
	
	p:flip_e()
	p:flip_f()
	while (p.f~=f) do
		push()
		p:flip_e()
		p:flip_f()
	end
		
	return result
end
document[[loopp(v:vertex) returns the loop of pos'es surrounding v]](loopp)
categorise(loopp,"mesh")
