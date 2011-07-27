--[[
	Mesh changing
--]]

module(...,package.seeall)

require "table"

local m1 = nil
local m2 = nil
local n = nil
local m = nil
local time = 0
local foo = false

function setup()
	m1 = fg.mesh.primitives.sphere()
	m2 = fg.mesh.primitives.icosahedron()
	m = m1
	
	n = fg.meshnode(m)
	fgu:add(n)	
end

function update(dt)
	time = time + dt
	if (time > .5) then
		if (foo) then
			n:setMesh(m1)
		else
			n:setMesh(m2)
		end
		
		foo = not foo
		time = 0
				
		--[[ Hmmm....? This should work...
		print("GO")	
		print(m==m1)
		print("==")
		if (m==m1) then m = m2 else m = m1 end
		print("SMO")
		n:setMesh(m)		
		time = 0
		print("END")
		--]]
	end
end

