--[[
	Tests creating a mesh and accessing and modifying its vertex positions.
--]]

module(...,package.seeall)

local m = {}
function setup()
	print("> setup()")
	-- print("icosahedron: " .. tostring(fg.mesh.primitives.icosahedron))
	m = fg.mesh.primitives.icosahedron()
	print(m)
	print(type(m))
	print("Mesh vertex information: ")
	for v in m:vertices() do
		print("--"); 
		print(v)
		print("v.pos = ", tostring(v.pos))
		print("v.pos.x = ", tostring(v.pos.x))
		print("setting v.pos.x to pi ...")
		v.pos.x = math.pi
		print("new v.pos = ", v.pos)
		print("randomising position ...")
		v.pos.x, v.pos.y, v.pos.z = math.random(),math.random(),math.random()
		print("new v.pos = ", v.pos)
	end
	print("< setup()")
end

local t = 0
function update(dt)
	t = t + dt
	-- print("update ... t=" .. t)
end


