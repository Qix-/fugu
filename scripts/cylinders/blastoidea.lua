module(...,package.seeall)

local m = {}
local node = {}
local stalk = {}

function setup()
	-- The stalk
   	stalk = new_stalk(10,1.,.6,.7)

	m = stalk:getMesh()
	-- Node for the mesh
	node = fg.meshnode(m)
	fgu:add(node)
end

local t = 0
function update(dt)
	t = t + dt
end

function new_stalk(n,width,length,meander)
	local obj = {
		numBeads = n,
		beadWidth = width * 10,
		beadLength = length,
		meanderParam = meander,

		dl = length * .25,
		dw = width * 9
	}

	obj.getMesh = function(self)
	  bert = fg.turtle()
	  bert:setCarrierMode(0)

	  -- Create the cylinder
	  width = self.beadWidth
	  length = self.beadLength
	  bert:setScale(self.beadWidth)
	  bert:beginCylinder()
	  bert:move(length)
  	  bert:setScale(width)

	  for i = 2, self.numBeads, 1 do
	  	bert:addPoint()

		bert:move(self.dl * .5)
		bert:setScale(self.dw)
		bert:addPoint()
		bert:move(self.dl * .5)

	  	width = self.beadWidth * fg.random(0.91,1.09)
	  	length = self.beadLength * fg.random(0.91,1.09)
		bert:setScale(width)
		bert:addPoint()
		bert:move(length)
		bert:setScale(width)
	  end
	  bert:endCylinder()
	  return bert:getMesh(1,self.numBeads*5)
	end

	obj.update = function(self,dt)
	end

	return obj
end
