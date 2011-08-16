--[[
	Tests node transforms
--]]

module(...,package.seeall)
require "table"

-- constants
local branches = 5
local depth = 3

-- Shorthand for transforms
function R(rad,x,y,z)
	local m = mat4()
	m:setRotateRad(rad,x,y,z)
	return m
end

function T(x,y,z)
	local m = mat4()
	m:setTranslate(x,y,z)
	return m
end

function S(x,y,z)
	local m = mat4()
	m:setScale(x,y,z)
	return m
end

local cyl = nil
local nodes = {}

function setup()	
	-- cyl is a cylinder of height 1, with base at (0,0,0)
	cyl = fg.mesh.primitives.cylinder(32)
	cyl:applyTransform(T(0,1,0))
	cyl:applyTransform(S(.1,.5,.1))
	
	local root = fg.meshnode(cyl)
	fgu:add(root)
	table.insert(nodes,root)
	branch(root,T(0,1,0),depth)
end

function branch(parent,tr,d)
	if (d<=0) then return end
	
	-- add an empty node to the end of the branch
	local empty = fg.node()
	empty:setTransform(tr*S(.6,.6,.6))
	fgu:add(empty)
	fgu:makeChildOf(parent,empty)
		
	local db = 2*math.pi/branches
	for i = 0,(branches-1) do
		local n = fg.meshnode(cyl)
		fgu:add(n)
		fgu:makeChildOf(empty,n)
		table.insert(nodes,n)
		n:setTransform(R(db*i,0,1,0)*R(math.pi/3,1,0,0))	
		branch(n,T(0,1,0),d-1)
	end
end

function update(dt) 
	time = fgu:time()
	-- rotate all nodes	
	table.foreach(nodes, 
		function(i,n) 
			local s = 1+0.003*math.sin(time+.7*i)
			n:setTransform(n:getTransform()*R(dt,0,1,0)*S(s,s,s))
		end
	)
	--[[
	y = math.sin(.5*fgu.t * math.pi)
	y = y*y*math.abs(y)
	n1.transform:setTranslate(0,y,0)
	n3.transform:setRotateRad(fgu.t,math.cos(fgu.t),1+math.sin(fgu.t),1)
	--]]	
end

