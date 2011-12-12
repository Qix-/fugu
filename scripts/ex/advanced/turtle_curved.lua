--[[
	Demonstrating curved cylinders with the turtle
	Also demonstrates add_slider
	JW+BP 12/2011
--]]

module(...,package.seeall)

local n = nil -- node to hold the mesh

-- global params of the thing
mean_length = 15
root_width = 2.
num_roots = 6
meander = .5

function setup()
	-- connect some sliders to the params
	add_slider {var="mean_length", value=mean_length, low=0, high=30}
	add_slider {var="root_width", value=root_width, low=.1, high=4}
	add_slider {var="num_roots", value=num_roots, low=3, high=9}
	add_slider {var="meander", value=meander, low=0, high=1}
	
	-- build it!
	local t = turtle()
	t:pitch(-pi/2)
	build_thing(t,mean_length,root_width,num_roots,meander)	
	n = meshnode(t:mesh())
	fgu:add(n)
end

function update()
	-- rebuild it every frame
	local t = turtle()
	t:pitch(-pi/2)
	build_thing(t,mean_length,root_width,num_roots,meander)
	n:set_mesh(t:mesh())
end

-- Create a thing out of curved roots..
function build_thing(t,mean_length,width,num_roots,meander)
	for i = 1, num_roots do
		-- create a root
		length = gauss(mean_length,.2)			
		t:push()
		t:roll(pi / num_roots * 2 * i + .03)
		build_root(t, length,width,meander)			
		t:pop()
	end
end

-- Create a curved root
function build_root(t,root_length,width,meander)
	local step_length  = root_length / 8	

	-- First the turn
	t:set_scale(width)
	t:set_stiffness(0.3,0.3)
	t:begin_cylinder()
	t:move(width)
	t:pitch(pi/4)
	t:move(width)
	t:set_scale(width * .7)
	t:add_point(2)
	t:move(width)
	t:pitch(pi/4)
	t:move(width)
	t:set_scale(width * .5)
	
	-- Now the wiggly bit
	local total_distance = 0
	local rate_cur = 0
	local wiggle = .2
	t:add_point(2)
	t:move(step_length)
	total_distance = total_distance + step_length
	while (total_distance < root_length) do		
		t:yaw(rate_cur/2)
		t:add_point(2)
		pos = t:get_position()
		rate_cur = rate_cur + meander * noise(pos.x*wiggle,pos.y*wiggle,pos.z*wiggle)
		--frac_sum(pos.x, pos.y, pos.z, 1, 1)
		t:yaw(rate_cur/2)
		t:move(step_length)
		total_distance = total_distance + step_length
	end
	t:set_scale(0.001)
	t:yaw(rate_cur/2)
	t:end_cylinder(2)		
end
