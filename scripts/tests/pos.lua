--[[
-- Demonstrates navigating a mesh using fg.pos
--]]

module(...,package.seeall)

local m = nil
function setup()
	m = fg.mesh.primitives.sphere()	
	fgu:add(fg.meshnode(m))
	
	for i=1,3 do doit() end
end
 
function update(dt) end

function doit()
	local v = m:selectRandomVertex()
	local f = v:getAdjacentFace()
	local e = 0
	local r = function() return vec3(random(),random(),random()) end
	straightWalk(fg.pos(f,e,v),50,r(),r())
end

-- Do a straightish walk from a pos, of length n, colouring the vertices the specified colour 
function straightWalk(pos,n,cf,ct)
	for i=1,n do
		local t = (i-1)/(n-1)
		pos.v:setColour(cf*(1-t) + ct*t)
		pos:flipE() 
		pos:flipF() 
		pos:flipE()
		if (random() < .8) then pos:flipF() end		
		pos:flipV()
	end
end