--[[
	Demonstrates turtle usage and recursion
	JW+BP 12/2011
--]]

module(...,package.seeall)

len = 20
width = 1
its = 5
	
function setup()	
  local turtle = branch(len,width,its)

	fgu:add(meshnode(turtle:mesh()))
end

function branch(length,width,rec,t)
	if (t==nil) then 
		t = turtle() 
		t:pitch(-pi/2)
	end
	if rec > 0 then
		t:set_scale(width):yaw(pi/10)
		t:begin_cylinder()
		-- trunk
		t:move(length/2):yaw(-pi/10):add_point(3):yaw(-pi/10)

		t:push()
		t:yaw(pi/2)
		branch(length/2,width/2,rec-1,t)
		t:pop()

		t:push()
		t:yaw(-pi/2)
		branch(length/2,width/2,rec-1,t)
		t:pop()

		t:push()
		t:pitch(pi/2)
		branch(length/2,width/2,rec-1,t)
		t:pop()

		t:push()
		t:pitch(-pi/2)
		branch(length/2,width/2,rec-1,t)
		t:pop()

		t:move(length/5)
		t:set_scale(width/8)
		t:end_cylinder(3)
	end
	return t;
end
