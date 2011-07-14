-- fg.core.vec3
module(...,package.seeall)

--[[

vec3 is loosely modelled on GLSL vec3

example:
vec3 = require("fg.core.vec3")
a = vec3.new(1,2,3)
b = vec3.new(2,4,6)
print(a)
print(a+b*3)

--]]

local vec3 = {}
vec3.mt = {}

function new (x,y,z)
	local a = {x=x,y=y,z=z}
	setmetatable(a,vec3.mt)
	return a
end

function add (a,b)
	return new(a.x+b.x,a.y+b.y,a.z+b.z)
end

vec3.mt.__add = add

function subtract (a,b)
	return new(a.x-b.x,a.y-b.y,a.z-b.z)
end

vec3.mt.__sub = subtract

function multiply (a,k)
	if (type(k)~='number') then
		error("Multiplying a vector by a non-scalar!",2)
	else 
		return new(a.x*k,a.y*k,a.z*k)
	end
end

vec3.mt.__mul = multiply

vec3.mt.__index = function(v,key) 
	if (type(key)=="number") then
		if (key==0) then return v.x
		elseif (key==1) then return v.y
		elseif (key==2) then return v.z
		else return nil
		end
	else return nil
	end
end

vec3.mt.__tostring = function(v)
	return "" .. v.x .. "," .. v.y .. "," .. v.z;
end

function dot(a,b)
	return a.x*b.x + a.y*b.y + a.z*b.z
end


