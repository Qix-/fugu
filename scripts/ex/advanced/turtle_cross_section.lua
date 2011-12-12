--[[
	Demonstrates using a complex cross section within a generalised cylinder 
	JW+BP 12/2011
--]]

module(...,package.seeall)

local n = nil -- node to hold the mesh
local length = 2.225
local width = 1.5
local segs = 4
local ncps = 16
noise_amount = .4

function setup()
	add_slider {var="noise_amount", value=noise_amount, low=0, high=1}

  local t = make_extrusion(5,ncps,segs,noise_amount,0.,width,length,vec3(0,0,0),vec3(0,0,0))
	n = meshnode(t:mesh())
	fgu:add(n)
end

function update()
	local t = make_extrusion(5,ncps,segs,noise_amount,0.,width,length,vec3(0,0,0),vec3(0,0,0))
	n:set_mesh(t:mesh())
end

function make_extrusion(n,npcs,ncs,noise_amount,noise2,width,l,offset1,offset2)
	local t = turtle()
	t:pitch(-pi/2)
  t:set_carrier_mode(1)	
  local index = make_cs(npcs,noise_amount,offset,t)

	t:set_cross_section(1)
  t:set_scale(width)
	t:begin_cylinder()

	local rateCur = 0.0
	local rateCur2 = 0.00
	for i = 2, ncs - 1, 1 do
		t:move(l)

		pos = t:get_position() + offset2
		rateCur = rateCur + noise2 * frac_sum(pos.x, pos.y, pos.z, 1, 1)
		t:yaw(rateCur/2)
		t:pitch(rateCur2/2)
	
		index = make_cs(npcs,noise_amount,0,t)	
	
		t:set_cross_section(index)
		t:add_point(n)
		t:yaw(rateCur/2)
		t:pitch(rateCur2/2)
	end

	t:move(l)
	index = make_cs(npcs,noise_amount,0,t)

	t:set_cross_section(index)
	t:end_cylinder(n)
	return t
end

function make_cs(npoints,noise_amount,offset,t)
	t:push()
	
	local pos = t:get_position()
	local heading = t:get_heading()
	local up = t:get_up()
	t:set_frame_rel( vec3( cos(2*pi), sin(2*pi), 0 ),
										vec3( -sin(2*pi), cos(2*pi), 0 ),
										vec3( 0, 0, 1 ) )
	local cpos
	cpos = t:get_position()
	--cpos = cpos + offset
	local dis = noise_amount * frac_sum(cpos.x, cpos.y, cpos.z, 1, 1)
	t:set_frame(pos,heading,up)
	t:set_frame( vec3( cos(2*pi), sin(2*pi), 0 ) * (1+dis),
										vec3( -sin(2*pi), cos(2*pi), 0 ),
										vec3( 0, 0, 1 ) )
	t:begin_cross_section()
	t:set_frame(pos,heading,up)

	for i = 1, npoints - 2, 1 do
		t:set_frame_rel( vec3( cos(2*i*pi/npoints), sin(2*i*pi/npoints), 0 ),
											vec3( -sin(2*i*pi/npoints), cos(2*i*pi/npoints), 0 ),
											vec3( 0, 0, 1 ) )
		cpos = t:get_position()-- + offset
		dis = noise_amount * frac_sum(cpos.x, cpos.y, cpos.z, 3, 1.5)
		t:set_frame(pos,heading,up)
		t:set_frame( vec3( cos(2*i*pi/npoints), sin(2*i*pi/npoints), 0 ) * (1+dis),
									vec3( -sin(2*i*pi/npoints), cos(2*i*pi/npoints), 0 ),
									vec3( 0, 0, 1 ) )
		t:add_point()
		t:set_frame(pos,heading,up)
	end

	t:set_frame( vec3( cos(2*pi), sin(2*pi), 0 ),
												vec3( -sin(2*pi), cos(2*pi), 0 ),
												vec3( 0, 0, 1 ) )
	cpos = t:get_position()
	dis = noise_amount * frac_sum(cpos.x, cpos.y, cpos.z, 3, 1.5)
	t:set_frame(pos,heading,up)
	t:set_frame( vec3( cos(2*pi*(1-1/npoints)), sin(2*pi*(1-1/npoints)), 0 ) * (1+dis),
										vec3( -sin(2*pi*(1-1/npoints)), cos(2*pi*(1-1/npoints)), 0 ),
										vec3( 0, 0, 1 ) )
	local res = t:end_cross_section()
	t:pop()
	return res
end
