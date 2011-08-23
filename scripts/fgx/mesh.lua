--[[
-- Useful mesh functions 
--]]

module(...,package.seeall)

require 'table'

-- return a list of all vertices in a mesh
function vertexlist(mesh)
	local vl = {}
 	local all = mesh:selectAllVertices()
	for v in all.verts do
		table.insert(vl,v)
	end
	return vl
end
