turtle = fg.turtle

-- types
document[[turtle() creates a turtle.
	A turtle can be used either:
	- as a navigation tool 
	- or to create generalised cylinders
	Check the examples for more info.
	
	Member functions:
	move(dist) 
	yaw(angle)
	roll(angle)
	pitch(angle)	
	set_frame(pos,heading,up) -- sets the turtles position heading and orientation
	set_stiffness(s1,s2) -- controls the shape of the generalised cylinder 
	set_scale(scale) -- controls the shape of the generalised cylinder
	set_cross_section(index) -- controls the shape of the generalised cylinder 	
	push -- record current state/position
	pop -- restore to old state/position	
	set_carrier_mode(cm) -- cm=0 use linear interpolation, cm=1 use bezier interpolation 	
	begin_cross_section 
	end_cross_section 	
	begin_cylinder
	end_cylinder
	add_point(n)	
	get_position
	get_heading
	get_up
	get_frame	
	mesh():mesh -- generate and return the generalised cylinder	
]](turtle)
categorise(turtle, "mesh")
categorise(turtle, "transform")