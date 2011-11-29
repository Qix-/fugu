--[[
	Demonstrates navigating a mesh using a mesh pos() object
	Make sure vertex colouring is enabled in fugu to see the results.
--]]

module(...,package.seeall)

local m
function setup()
	m = sphere()	
	fgu:add(meshnode(m))	
	for i=1,3 do doit() end
end
 
function update(dt) end

-- choose a random pos() on a mesh, and do a walk from it
function doit()	
	local v = m:select_random_vertex()
	local f = v:get_adjacent_face()
	local e = 0	
	local p = pos(f,e,v) -- create a pos
	walk(pos(f,e,v),50,random_vec3(),random_vec3())
end

-- Do a walk from a pos, of length n, colouring the vertices the specified colour 
function walk(pos,n,cf,ct)
	for i=1,n do
		local t = (i-1)/(n-1)
		pos.v.c = cf*(1-t) + ct*t
		pos:flip_e() 
		pos:flip_f() 
		pos:flip_e()
		if (random() < .8) then pos:flip_f() end		
		pos:flip_v()
	end
end