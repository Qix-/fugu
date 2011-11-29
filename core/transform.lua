--[[
-- Various transformation related stuff
--]]

require 'core.pos'

-- Shorthand for common transformations

-- Rotate a number of radians around a specified axis
function R(radian,axis_or_x,y,z)
	if (z==nil) then
		return mat4():set_rotate_rad(radian,axis_or_x) 
	else
		return R(radian,vec3(axis_or_x,y,z))
	end
end

document[[R(rad,axis), R(rad,x,y,z) generates a matrix that will rotate a point a number of radians around the specified axis (vec3)]](R)
categorise(R,"transform")

-- Rotates vec from to vec to
function Rv(from,to)
	return mat4():set_rotate(from,to)
end
document[[Rv(a:vec3,b:vec3) generates a matrix that will rotate vector a to align with vector b]](Rv)
categorise(Rv,"transform")

-- Translate by some amount
function T(translation_or_x, y, z)
	if (z==nil) then
		return mat4():set_translate(translation_or_x)
	else
		return T(vec3(translation_or_x,y,z))
	end		
end
document[[T(vec3), T(x,y,z) generates a matrix that translates a point by the given vector]](T)
categorise(T,"transform")

-- Scale by some amount
function S(v_or_x,y,z)
	if (z==nil) then
		if (y==nil) then 
			return mat4():set_scale(v_or_x)
		else
			return mat4():set_scale(v_or_x,y,1)
		end	
	else
		return mat4():set_scale(v_or_x,y,z)	
	end
end

document[[S(s:vec3) generates a matrix that scales a point by the vector s]](S)
categorise(S,"transform")

function scalef(poslist,center,amount)
	-- first we build a list of all unique vertices
	-- so no vertex gets scaled more than once
	local unique = {}
	
	local function addUnique(v)
		for _,u in ipairs(unique) do 
			if u==v then return end
		end
		table.insert(unique,v) 
	end
	
	for _,f in fgx.pos.ifaces(poslist) do
		for i=1,3 do
			addUnique(f:v(i-1))
		end		
	end
	
	-- now we can scale the vertices
	for _,u in ipairs(unique) do
		u.p = center + (u.p - center)*amount
	end 
end
document[[scalef(poslist,center,amount): scales a list of faces around a specified point
	amount 	= 0 -> all vertices will be moved to the center point
   			= 1 -> all vertices will remain in place
   			in (0,1) -> faces will shrink
   			>1 -> faces will grow]](scalef) 
categorise(scalef,"transform")

function transformf(poslist,T)
	-- first we build a list of all unique vertices
	-- so no vertex gets scaled more than once
	local unique = {}
	
	local function addUnique(v)
		for _,u in ipairs(unique) do 
			if u==v then return end
		end
		table.insert(unique,v) 
	end
	
	for _,f in fgx.pos.ifaces(poslist) do
		for i=1,3 do
			addUnique(f:v(i-1))
		end		
	end
	
	-- now we can scale the vertices
	for _,u in ipairs(unique) do
		u.p = T*u.p
	end 
end
document[[transformf(poslist,T) applies transform T to each vertex in poslist]](transformf)
categorise(transformf,"transform")