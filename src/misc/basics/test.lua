module(...,package.seeall)

local m = {}
function setup()
	print("Setting up module")
	-- print("icosahedron: " .. tostring(fg.mesh.primitives.icosahedron))
	print(fg.mesh.primitives.icosahedron())	
end

local t = 0
function update(dt)
	t = t + dt
	print("update ... t=" .. t)
end


