--[[
	Demonstrates composing functions for animation.
--]]
module(...,package.seeall)

local function nth(n)
	return function(t) 
		v=1
		for i=1,n do 
			v = v*t
		end
		return v
	end
end

local function mult(x)
	return function(t) return t*x end 
end

local function vec_noise(v)
	return noise(v.x,v.y,v.z)
end

local time_multiplier = 3
local make_object
local objects = {}
function setup()
	local m = cube()
	local abssin = compose(abs,sin)
	local abscos = compose(abs,cos)	
	local functions = {
		abssin,
		abscos,
		compose(abssin,sin),
		compose(abscos,sin),
		compose(abs,nth(3),sin),
		compose(nth(4),sin), 
		compose(abs,nth(5),sin),
		function(t) return smoothstep(0,cos(t),abssin(t)) end,
		compose(abscos,mult(2)),
		compose(abscos,mult(2.5)),
		compose(abscos,mult(3)),
		compose(abscos,mult(pi)),
	}
	
	local index = 0
	local w = 4
	for x=-w,w do
		for z=-w,w do			
			local n = meshnode(m)
			fgu:add(n)
			objects[#objects+1] = make_object(n, vec3(x*2,0,z*2), 
				functions[1+index%#functions])			
			index = index+1
		end
	end			
end

function update(dt)
	foreach(objects, function(_,o) o:update(fgu.t*time_multiplier) end)
end

function make_object(node,pos,pulse_function)
	local obj = {n=node,p=pos,f=pulse_function}
	obj.update = function(self, time)
		self.n:set_transform(
			T(self.p)*S(self.f(time))
		)
	end
	return obj
end