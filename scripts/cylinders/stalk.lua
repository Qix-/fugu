module(...,package.seeall)

local m = {}
local node = {}

function setup()

	b = new_stalk(1.,1.,15,.06)

    donatello = fg.turtle()
	donatello:pitch(-math.pi*.5)

	b:build(donatello)

	node = fg.meshnode(donatello:getMesh())
	fgu:add(node)

end

local t = 0
function update(dt)

end

function new_stalk(length,width,n,bendy)
	local obj = {
		numBeads = n,
		beadWidth = width,
		beadLength = length,

		dl = length * .1,
		dw = width * .5,

		b = bendy
	}

	obj.build = function(self, bert)
	  bert:setCarrierMode(0)

	  -- Create the cylinder
	  width = self.beadWidth
	  length = self.beadLength
	  bert:setScale(self.beadWidth)
	  bert:beginCylinder()
	  bert:move(self.dl)
  	  --bert:setScale(width)

	  for i = 2, self.numBeads, 1 do
	  	bert:addPoint(1)

        bert:yaw(self.b)
--		bert:pitch(random(-.07,.07))
		bert:move(self.dl * .5)
		bert:setScale(self.dw)
		bert:addPoint(1)
		bert:move(self.dl * .5)

	  	width = randomN(self.beadWidth, self.beadWidth * .03)
	  	length = randomN(self.beadLength, self.beadLength * .2)
		bert:setScale(width)
		bert:addPoint(1)

		bert:setScale(width)
		bert:move(.2*length)
		bert:addPoint(2)
		bert:move(length)
	  end
	  bert:endCylinder(2)
	end

	obj.update = function(self,dt)
	end

	return obj
end

