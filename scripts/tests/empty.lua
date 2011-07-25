--[[
	Tests node transforms
--]]

module(...,package.seeall)

local n1, n2, n3
function setup()	
	
	n1 = fg.node() -- blank transform node
	n2 = fg.node()
	n3 = fg.node()
			
	fgu:add(n1)
	fgu:add(n2)
	fgu:add(n3)
	
	fgu:makeChildOf(n1,n3)
	fgu:makeChildOf(n3,n2)	
	
	n2.transform:setTranslate(3,0,0)		
end


function update(dt) 
	y = math.sin(.5*fgu.t * math.pi)
	y = y*y*math.abs(y)
	n1.transform:setTranslate(0,y,0)
	n3.transform:setRotateRad(fgu.t,math.cos(fgu.t),1+math.sin(fgu.t),1)	
end