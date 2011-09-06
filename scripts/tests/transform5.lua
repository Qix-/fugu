--[[
	Tests adding new nodes and transforms
--]]

module(...,package.seeall)

local mesh 
local nodes = {}

function setup()
	mesh = fg.mesh.primitives.icosahedron()	
	nodes[#nodes+1] = fg.meshnode(mesh)		
	fgu:add(nodes[#nodes])	
end

local ticker = 0
function update(dt)
	ticker = ticker + dt
	if (ticker > 1) then
		ticker = 0
		nodes[#nodes+1] = fg.meshnode(mesh)
		nodes[#nodes]:setTransform(mat4():setTranslate(0,1,0))
		fgu:add(nodes[#nodes])
		fgu:makeChildOf(nodes[#nodes-1],nodes[#nodes])
	end
	
	for _,n in ipairs(nodes) do
		n:setTransform(n:getTransform()*mat4():setRotateRad(dt,0,1,0))
	end	
end


