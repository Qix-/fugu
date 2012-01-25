module(...,package.seeall)

local m = nil -- the mesh
local verts = nil -- stores all vertices on the sphere

function setup()
	m = sphere()	
	verts = vertexlist(m)
	
	fgu:add(meshnode(m))
	
	m:sync()
	extrude(m,verts[1],.5)
	-- recompute the normals if needed
	m:sync()	
end

function update(dt)
  
end