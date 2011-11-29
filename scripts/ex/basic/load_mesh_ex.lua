--[[
	Demonstrates loading a mesh from an .obj file.
--]]

module(...,package.seeall)

local n
function setup()
	local m = load_mesh("../assets/suzanne.obj")
	n = meshnode(m)
	fgu:add(n)	
end

-- do nothing
function update(dt) 
	n:set_transform(R(fgu.t,0,1,0)*S(1+.2*sin(5*fgu.t)))
end
