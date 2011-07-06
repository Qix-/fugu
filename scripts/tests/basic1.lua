--[[
	Tests creating a mesh and accessing and modifying its vertex positions.
--]]

module(...,package.seeall)

local m = {}
function setup()
	print("> setup()")
	m = fg.mesh.primitives.icosahedron()
	print(m)
	print(type(m))
	print("Mesh vertex information: ")
	for v in m:selectAllVertices().all do
		print("--"); 
		print(v)
		p = v:getPos()
		print("v.pos = ", tostring(p))
		print("setting v.pos.x to pi ...")		
		v:setPos(math.pi,p.y,p.z)
		print("new v.pos = ", v:getPos())
		print("randomising position ...")
		v:setPos(math.random(),math.random(),math.random())
		print("new v.pos = ", v:getPos())
	end
	print("< setup()")
end

local t = 0
function update(dt)
	t = t + dt
	-- print("update ... t=" .. t)
end


