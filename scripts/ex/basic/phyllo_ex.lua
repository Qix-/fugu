--[[
	Demonstrates arranging elements in a phyllotactic pattern.
	BP&jonmc 16/11/2011
--]]

module(...,package.seeall)

local object_node
local thorn_mesh

local A = 4		-- sphere radius
local B = 10    	-- prolate
local elemR = 0.7 	-- element radius
local N = 550		-- number of elements
local phi = 2.39982772	-- golden ratio
local spa = 0.7554	-- segment area

function setup()
	object_node = node()
	fgu:add(object_node)
	object_node:set_transform(R(-pi/2,1,0,0))
	
	-- create the thorn
	thorn_mesh = cone(elemR,0.001,6)
	-- colour it black at the base, white at the tip
	each(vertexlist(thorn_mesh), 
		function(v)			
			local l = v.p.y
			v.c = vec3(l,l,l)
		end)
	thorn_mesh:apply_transform(R(pi,1,0,0))	
	
	-- finally build the whole form
	build_form(2,sqrt(spa/pi),N,.01)
end

-- update just rotates the form
local counter = 0
function update(dt)
	object_node:set_transform(R(.1*fgu.t,0,1,0)*R(-pi/2,1,0,0))
end

function build_form(length,width,n,meander)
	-- the turtle simplifies using compound transforms
	local t = turtle()
	
	for i = 1, n do
		-- create a root at points[i]
		local actual_length = gauss(length,.2)
		
		t:roll(phi)
		t:push()
		local elip_h = find_ellipse_h(A, B, spa * i);
		t:pitch(pi)
		t:move(elip_h)
		t:pitch(pi/4)
		t:move(A * sqrt(1 - (sqr(elip_h)/sqr(B))))
		t:pitch(get_angle_from_h(A, B, elip_h))
		
		-- node for this element
		local no = meshnode(thorn_mesh)							
		no:set_transform(t:get_frame()*S(width,actual_length,width))
		fgu:add(no)
		fgu:make_child_of(object_node,no)
		
		t:pop()
	end
	
end
