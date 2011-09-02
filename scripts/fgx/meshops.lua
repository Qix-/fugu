--[[
--  Handy mesh operations.
--]]

module(...,package.seeall)

require 'fgx.extrude'
require 'fgx.nloop'

extrude = fgx.extrude.extrude
extrudeAndScale = fgx.extrude.extrudeAndScale 

-- inset the edge loop around a vertex
function inset(mesh,vertex,scale)
	local direction = vertex.n	
	local ddotd = dot(direction,direction)	
	local vdotd = dot(vertex.p,direction)
	local vp = vertex.p
		
	-- do a basic extrude
	fg.extrude(mesh,vertex,1,direction,0)
	
	-- then scale the vertex positions
	local posfan = fgx.nloop.loopp(vertex)
	for _,pos in ipairs(posfan) do
		-- to get the outside vertex we need to flipV the pos
		pos:flipV()
		n = pos.v
		-- n.p = n.p + direction*(magnitude + (vdotd-dot(n.p,direction))/ddotd)		
		n.p = lerp(vp,n.p,scale)
		pos:flipV()
	end
	return posfan		
end

-- flatten a list of vertices so they align on the plane specified by position and normal
function flattenvl(mesh,vertexlist,position,normal)
	table.foreachi(vertexlist, function(i,v)
		v.p = v.p - normal*dot(v.p-position,normal)
	end)
end