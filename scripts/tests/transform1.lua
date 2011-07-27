--[[
	Tests node transforms
--]]

module(...,package.seeall)

local n1, n2, n3
function setup()
	mesh = fg.mesh.primitives.icosahedron()
	
	n1 = fg.meshnode(mesh)
	n2 = fg.meshnode(mesh)
	n3 = fg.node() -- blank transform node
			
	fgu:add(n1)
	fgu:add(n2)
	fgu:add(n3)
	
	fgu:makeChildOf(n1,n3)
	fgu:makeChildOf(n3,n2)	
	
	n2.transform:setTranslate(4,0,0)		
end

function update(dt) 
	n1.transform:setTranslate(0,2*math.sin(fgu.t * math.pi),0)
	n3.transform:setRotateRad(fgu.t,0,1,1)
end


