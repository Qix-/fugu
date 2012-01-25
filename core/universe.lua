
node = fg.node
meshnode = fg.meshnode
universe = fg.universe

document[[A universe stores all nodes. 
	There is only one universe, called "fgu". Example usage:
	fgu:add(n) -- adds a node n to the universe
	fgu:make_child_of(parent,child) -- spatially links child to parent
	fgu.t -- returns the age of the universe (in seconds)   
	]](universe)

document[[A node is a positionable and orientable object in a universe.
	Example usage:
	n = node()
	n:set_transform(T(3,0,0)) -- translate by 3 units in the x direction	
	]](node)

document[[A meshnode is a node that references and transforms a mesh. Multiple meshnodes can reference the same mesh.
	Example usage:
	m = cube()
	n = meshnode(m)
	n:set_transform(R(half_pi,vec3(0,1,0))) -- rotate the mesh 45 degrees around the y axis
	m2 = dodecahedron()
	n:set_mesh(m2) -- change the mesh this node is referencing
	]](meshnode)

foreach({universe,node,meshnode}, function(_,f) categorise(f,"universe")
 end)
