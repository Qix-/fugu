--[[
	Demos hierarchical transforms
--]]

module(...,package.seeall)

-- constants
local branches = 5
local depth = 3


local cyl = nil
local nodes = {}

function setup()	
	-- cyl is a cylinder of height 1, with base at (0,0,0)
	cyl = cylinder(32)
	cyl:apply_transform(T(0,1,0))
	cyl:apply_transform(S(.1,.5,.1))
	
	local root = meshnode(cyl)
	fgu:add(root)
	table.insert(nodes,root)
	branch(root,T(0,1,0),depth)
end

function branch(parent,tr,d)
	if (d<=0) then return end
	
	-- add an empty node to the end of the branch
	local empty = node()
	empty:set_transform(tr*S(.6,.6,.6))
	fgu:add(empty)
	fgu:make_child_of(parent,empty)
		
	local db = 2*pi/branches
	for i = 0,(branches-1) do
		local n = meshnode(cyl)
		fgu:add(n)
		fgu:make_child_of(empty,n)
		table.insert(nodes,n)
		n:set_transform(R(db*i,0,1,0)*R(pi/3,1,0,0))	
		branch(n,T(0,1,0),d-1)
	end
end

function update(dt) 
	-- rotate all nodes		
	foreach(nodes, 
		function(i,n) 
			local s = 1+0.003*sin(fgu.t+.7*i)
			n:set_transform(n:get_transform()*R(2*dt,0,1,0)*S(s,s,s))
		end
	)
end

