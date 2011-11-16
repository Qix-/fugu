--[[
-- Various extrusion operations
-- @author ben
--]]

function extrude(mesh,vertex,magnitude)
	fg._extrude(mesh,vertex,1,vertex.n,magnitude)
end
-- documentation is combined with the overloaded version
-- document[[extrude(m:mesh,v:vertex,magnitude) extrudes all faces neighbouring v in the direction of the vertex normal]](extrude)



function extrude(mesh,vertex,direction,magnitude)
	local ddotd = dot(direction,direction)
	local vdotd = dot(vertex.p,direction)
	local vp = vertex.p + direction*magnitude
	
	-- do a basic extrude
	fg._extrude(mesh,vertex,1,direction,1)
	
	-- then change the vertex positions, so the faces are oriented correctly
	vertex.p = vp	
	
	local posfan = loopp(vertex)
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
document[[extrude(m:mesh,v:vertex,[dir:vec3],magnitude)
	Extrudes all neighbouring faces in the direction given (or the normal if dir is omitted), and re-orients the
	faces to align perpendicularly along the extrusion axis
	Note: Assumes direction is normalised. 
	Returns the pos loop at the end of the extrusion]](extrude)

categorise(extrude,"mesh")
	
function extrudeAndScale(mesh,vertex,direction,magnitude,scale)
	local ddotd = dot(direction,direction)	
	local vdotd = dot(vertex.p,direction)
	local vp = vertex.p + direction*magnitude
	
	-- do a basic extrude
	fg._extrude(mesh,vertex,1,direction,1)
	
	-- then change the vertex positions, so the faces are oriented correctly
	vertex.p = vp	
	local posfan = loopp(vertex)
	
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
extrude_and_scale = extrudeAndScale
document[[extrude_and_scale(m:mesh,v:vertex,dir:vec3,magnitude,scale) extrudes and scales the faces adjacent to v]](extrude_and_scale)
categorise(extrude_and_scale,"mesh")