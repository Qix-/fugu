--[[
	Tests all the math functionality of fg.
	
	Basically just to make sure all the functions work in lua, the 
	actual return values are unimportant for these tests.
--]]

module(...,package.seeall)

function test(name,condition)
	if (not condition) then 
		print("[" .. name .. "] FAILED")
	else
		print("[" .. name .. "]")
	end
end

function test(name)
	print("[" .. name .. "]")
end

function setup()
	print("** testing fg maths **")
	
	print("** testing functions **")
		
	local x = .4 -- the variable to one var functions 
	
	test("min", min(3.4,5)==3.4)
	test("lerp", lerp(1,5,.5)==3)
	test("mix", mix(-2,2,.5)==0)
	test("clamp", clamp(3,7,200.1)==7)
	test("step", step(4,3)==0 and step(4,5)==1)
	test("pulse", pulse(0,4,-1)==0 and pulse(0,4,2)==1 and pulse(0,4,5)==0)
	test("smoothstep") smoothstep(0,5,.2)
	-- test("catmullSpline", ... ) 
	test("bias") bias(3,x) 
	test("gain") gain(3,x)
	test("gamma") gamma(3,x)
	test("invSqrt") invSqrt(x)
	test("sqrt", sqrt(9)==3)
	test("sqr", sqr(3)==9)
	test("sign", sign(10)==1 and sign(-3)==-1)
	
	local y = .5
	local z = .8
	test("noise") noise(x)
	test("noise") noise(x,y)
	test("noise") noise(x,y,z)
	
	test("fracSum") fracSum(x,y,z,4,1)
	test("turbulence") turbulence(x,y,z,4,1)	
	test("random") random() random(0,10) randomN(5,2)
	
	print("** testing vec3 **")
	
	test("construction")
	local x = vec3(1,0,0)
	local y = vec3(0,1,0)
	local z = vec3(0,0,1)	
	test("print") print(x)
	test("component access")	
	local tmp = vec3(3,4,5)
	print(tmp)
	tmp.y = 10
	print(tmp)
	tmp.x = tmp.z*tmp.x
	test("rgb component access", tmp==vec3(tmp.r,tmp.g,tmp.b))
	test("basic ops") 
	tmp = x+y 
	tmp = z-y 
	tmp = -z			
	tmp = x*2.678
	tmp = z/2.678	
	tmp = x-y+z*2
	test("length", length(x)==1 and length(x+y)==sqrt(2))
	test("distance", distance(x,y)==sqrt(2))
	test("dot", dot(x,y)==0 and dot(x,x)==1)
	test("cross", cross(x,y)==z)
	test("normalise", normalise(x)==x and length(normalise(x+y*2-z))==1)			
	print("** testing mat4 **")
	test("construction")
	local id = mat4() -- uninitialised matrix
	id:setBasis(vec3(1,0,0),vec3(0,1,0),vec3(0,0,1))
	local m = mat4() 
	m:setBasis(x+y*2, y*4-z, x-y+z*2) -- set x,y,z	
	test("print")
	print(m)	
	test("ops")	
	local tmp = m + m
	tmp = tmp - m
	tmp = -(m*4)
	tmp = tmp*tmp	
	local v = id * vec3(2,3,4)
	print(v)	
	test("element access")
	for r=0,3 do
		 local row = tmp:get(r,0),tmp:get(r,1),tmp:get(r,2),tmp:get(r,3)
	end
	test("transformation functions")
	local tmp = mat4()
	tmp:setRotateRad(math.pi/4,0,0,1)
	tmp:setRotateRad(math.pi/4,vec3(0,0,1))
	tmp:setTranslate(0,1,0)
	tmp:setTranslate(vec3(0,1,0))
	tmp:setScale(2,1,1)
	tmp:setScale(vec3(2,1,1))

	local r1 = mat4()
	r1:setRotateRad(math.pi/4,0,0,1)
	local r2 = mat4()
	r2:setRotateRad(-math.pi/4,0,0,1)
	test("rotate", r1*r2==id)
	test("rotate vec", r1*vec3(1,0,0)==normalise(vec3(1,1,0)))
			
	print("** finished! **")
end

function update(dt) end


