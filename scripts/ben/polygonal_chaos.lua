module(...,package.seeall)

local the_mesh
local the_vertices
local get_flux, get_colour, tighten
local counter, tighten_amount

function setup()
	the_mesh = sphere()
	fgu:add(meshnode(the_mesh))
	the_vertices = vertexlist(the_mesh)
	counter = 10
	tighten_amount = 0
end

function update(dt)
	local speed = fgu.t*0.01
	for i=1,#the_vertices do
		local v = the_vertices[i]
		local vp = v.p
		v.c = get_colour(vp.x,vp.y,vp.z,speed*fgu.t)
		v.p = v.p + get_flux(vp.x,vp.y,vp.z,speed*fgu.t)		
	end
	
	counter = counter - 1
	if (counter < 0) then 
		tighten(tighten_amount)
		counter = 10
		tighten_amount = min(1,tighten_amount + 0.0001)
	end
end

-- Below lies the definition of some helper functions for this script.

get_flux = function(x,y,z,t)
  local detail = 4
	x = detail*x
	y = detail*y
	z = detail*z
	return vec3(
		noise (x,y,z+t),
		noise (x+t,y,z),
		noise (x,y+t,z)
		)*0.002
end

get_colour = function(x,y,z,t) 
	local detail = 1
	x = detail*x
	y = detail*y
	z = detail*z
	return vec3(
		.5 + .5*noise(x,0,0),
		.5 + .5*noise(0,y,0),
		.5 + .5*noise(0,0,z)) 
end

tighten = function(amount)
	each(the_vertices, function(v)
		local neighbours = nearbyv(the_mesh,v,1)
		local p = vec3(0,0,0)
		each(neighbours, function(n) 
			p = p + n.p
		end)
		p = p/#neighbours
		v.p = lerp(v.p,p,amount)
	end)
end