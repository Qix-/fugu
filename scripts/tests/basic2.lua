--[[
	Tests accessing the fgu universe
--]]

module(...,package.seeall)

-- uncomment this to cause a clash of the unique fgu object
-- fgu = 300

local m = {}
function setup()
	print("> setup()")
	print("fgu: " .. tostring(fgu))
	print("adding a new mesh")
	m = fg.mesh.primitives.icosahedron()
	fgu:addMesh(m)
	print("< setup()")
end

-- do nothing
function update(dt) end


