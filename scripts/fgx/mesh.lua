--[[
-- Useful mesh functions 
--]]

module(...,package.seeall)

require 'table'
require 'fgx.nloop'
require 'fgx.pos'

-- return a list of all vertices in a mesh
function vertexlist(mesh)
	local vl = {}
 	local all = mesh:selectAllVertices()
	for v in all.verts do
		table.insert(vl,v)
	end
	return vl
end

-- applies f to each vertex in the mesh
function foreachv(mesh,f)
	local all = mesh:selectAllVertices()
	for v in all.verts do
		f(v)
	end
end

-- return a list of vertices within edge distance n to v
function nearbyv(mesh, v, n)
	local vl = {}
	local near = fg.getVerticesWithinDistance(mesh,v,n)
	for v in near.verts do
		table.insert(vl,v)
	end
	return vl
end

-- convert a list of vertices to a list of all adjacent faces
function tofaces(vl)
	local unique = {}
	
	local function addUnique(f)
		for _,u in ipairs(unique) do 
			if u==f then return end
		end
		table.insert(unique,f)
	end
	
	for i,v in ipairs(vl) do		
		local pl = fgx.nloop.loopp(v)
		for _,f in fgx.pos.ifaces(pl) do
			addUnique(f)
		end
	end
		
	return unique
end