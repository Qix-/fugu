--[[
-- Various extrusion operations
-- @author ben
--]]

module(...,package.seeall)

require 'fgx.nloop'

-- Extrudes all neighbouring faces in the direction of the vertex normal
function extrude(mesh,vertex,magnitude)
	fg.extrude(mesh,vertex,1,vertex.n,magnitude)
end

-- Extrudes all neighbouring faces in the direction given, and re-orients the
-- faces to align perpendicularly along the extrusion axis
-- Note: Assumes direction is normalised. 
-- Returns the pos loop at the end of the extrusion
function extrude(mesh,vertex,direction,magnitude)
	local ddotd = dot(direction,direction)
	local vdotd = dot(vertex.p,direction)
	local vp = vertex.p + direction*magnitude
	
	-- do a basic extrude
	fg.extrude(mesh,vertex,1,direction,1)
	
	-- then change the vertex positions, so the faces are oriented correctly
	vertex.p = vp	
	
	local posfan = fgx.nloop.loopp(vertex)
	for _,pos in ipairs(posfan) do
		-- to get the outside vertex we need to flipV the pos
		pos:flipV()
		n = pos.v
		n.p = n.p + direction*(magnitude + (vdotd-dot(n.p,direction))/ddotd)
		pos:flipV()	 
	end
	return posfan
	
	-- Old: also expanded the faces
	-- vcg::Point3d exp = (newCenter-p->P())*-expand;
	-- p->P() += exp;				
end

-- Extrudes all neighbouring faces in the direction given, re-orients the
-- faces to align perpendicularly along the extrusion axis, and scales the faces 
-- Note: Assumes direction is normalised. 
-- Returns the pos loop at the end of the extrusion
function extrudeAndScale(mesh,vertex,direction,magnitude,scale)
	local ddotd = dot(direction,direction)	
	local vdotd = dot(vertex.p,direction)
	local vp = vertex.p + direction*magnitude
	
	-- do a basic extrude
	fg.extrude(mesh,vertex,1,direction,1)
	
	-- then change the vertex positions, so the faces are oriented correctly
	vertex.p = vp	
	local posfan = fgx.nloop.loopp(vertex)
	
	for _,pos in ipairs(posfan) do
		-- to get the outside vertex we need to flipV the pos
		pos:flipV()
		n = pos.v
		n.p = n.p + direction*(magnitude + (vdotd-dot(n.p,direction))/ddotd)		
		n.p = lerp(vp,n.p,scale)
		pos:flipV()
	end
	return posfan					
end