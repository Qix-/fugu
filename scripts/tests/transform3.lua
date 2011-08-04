--[[
	Tests node transforms
--]]

module(...,package.seeall)

-- constants
local branches = 3
local depth = 2

-- Shorthand for transforms
function R(rad,x,y,z)
	local m = fg.mat4()
	m:setRotateRad(rad,x,y,z)
	return m
end

function T(x,y,z)
	local m = fg.mat4()
	m:setTranslate(x,y,z)
	return m
end

function S(x,y,z)
	local m = fg.mat4()
	m:setScale(x,y,z)
	return m
end

local cyl = nil

function setup()	
	-- cyl is a cylinder of height 1, with base at (0,0,0)
	cyl = fg.mesh.primitives.cylinder(32)
	cyl:applyTransform(T(0,1,0))
	cyl:applyTransform(S(.1,.5,.1))
	
	local root = fg.meshnode(cyl)
	fgu:add(root)
	branch(root,T(0,1,0),depth)
end

function branch(parent,tr,d)
	if (d<=0) then return end
	
	local db = 2*math.pi/branches
	for i = 0,(branches-1) do
		n = fg.meshnode(cyl)
		fgu:add(n)
		fgu:makeChildOf(parent,n)
		n.transform = tr*S(.6,.6,.6)*R(db*i,0,1,0)*R(math.pi/3,1,0,0)	
		branch(n,T(0,1,0),d-1)
	end
end

function update(dt) 
	--[[
	y = math.sin(.5*fgu.t * math.pi)
	y = y*y*math.abs(y)
	n1.transform:setTranslate(0,y,0)
	n3.transform:setRotateRad(fgu.t,math.cos(fgu.t),1+math.sin(fgu.t),1)
	--]]	
end

