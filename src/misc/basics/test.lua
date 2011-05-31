module(...,package.seeall)

function setup()
	print("Setting up module")
	print("fg.min(5.5,2.1)=" .. fg.min(5.5,2.1))
end

local t = 0
function update(dt)
	t = t + dt
	print("update ... t=" .. t)
end


