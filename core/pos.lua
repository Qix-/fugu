--[[
-- pos realted help and helper functions
--]]

require 'table'

-- move pos into main namespace
pos = fg.pos

-- help

document[[a pos is a (vertex,edge,face) triple that uniquely identifies a position on a mesh.
	Constructors:
	pos(f,e,v) -- construct from a face, edge(int) and vertex
	pos(f,v) -- construct from a face and vertex	
	Attributes:
	v -- the vertex
	e -- the edge (an integer from 0 to 2)
	f -- the face
	Properties:
	flip_v, flip_e, flip_f -- navigate the mesh
	
	A pos can walk around a mesh by calling flip_v, flip_e or flip_f. 
	flip_v, for instance, changes the pos by selecting the pos that has the same e and f, but differs in v.	
]](pos)
categorise(pos,"mesh")

-- helpers

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
document[[capov(cap:poslist) returns the outermost vertices of a cap (a fan of pos'es)]](capov)
categorise(capov,"mesh")

-- get an iterator over all the faces in this poslist 
function ifaces(poslist)
	local state = {list = poslist, last = poslist[1], index = 1}	
	return faces_iter, state
end
document[[ifaces(faces:poslist) returns an iterator over the faces in this poslist]](ifaces)
categorise(ifaces,"mesh")

function faces_iter (state) -- local
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