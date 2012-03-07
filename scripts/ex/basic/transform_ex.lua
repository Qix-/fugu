--[[
	Demonstrates the basic node transformations.
	BP 16/11/2011
--]]

module(...,package.seeall)

local n1, n2, n3
function setup()	
	
	n1 = node() -- blank transform node
	n2 = meshnode(icosahedron())
	n3 = meshnode(dodecahedron())
			
	fgu:add(n1)
	fgu:add(n2)
	fgu:add(n3)
	
	fgu:make_child_of(n1,n3)
	fgu:make_child_of(n3,n2)	
	
	n2:set_transform(T(3,0,0))
end

function update(dt) 	
	local t = fgu.t
	local y = sin(.5*t * pi)
	y = y*y*abs(y)
	n1:set_transform(T(0,y,0))
	n3:set_transform(R(t,cos(t),1+sin(t),1))
end