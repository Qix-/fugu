module(...,package.seeall)

local m = {}
local node = {}

function setup()

	b = new_brachioles(.15,1.2,3)

    donatello = fg.turtle()
	donatello:pitch(-math.pi*.5)

	b:build(donatello)

	node = fg.meshnode(donatello:getMesh())
	fgu:add(node)

end

local t = 0
function update(dt)

end

function new_brachioles(width,length,n)
	local obj = {
		segWidth = width,
		segLength = length,
		numSegs = n,

		l = 0.2,
		dl = length * .2,
		meander = 0.10,
		meander2 = 0.05
	}

	obj.build = function(self, bert)
	    bert:pushState()
		
		-- create cross section
		bert:setStiffness(.3,.3)
		bert:setFrame(vec3(1+self.l,0.,0.),vec3(0.,1.,0.),vec3(0.,0.,1.))
		bert:beginCrossSection()

		bert:setFrame(vec3(0,1,0),vec3(-1,0.,0.),vec3(0.,0.,1.))
		bert:addPoint()

		bert:setFrame(vec3(-1-self.l,0,0),vec3(0,-1,0),vec3(0.,0.,1.))
		bert:addPoint()

		bert:setFrame(vec3(0,-1,0),vec3(1,0,0),vec3(0.,0.,1.))
		bert:addPoint()
		cs1 = bert:endCrossSection()

		-- create cross section
		bert:setFrame(vec3(1-self.l,0.,0.),vec3(0.,1.,0.),vec3(0.,0.,1.))
		bert:beginCrossSection()

		bert:setFrame(vec3(0,1,0),vec3(-1,0.,0.),vec3(0.,0.,1.))
		bert:addPoint()

		bert:setFrame(vec3(-1+self.l,0,0),vec3(0,-1,0),vec3(0.,0.,1.))
		bert:addPoint()

		bert:setFrame(vec3(0,-1,0),vec3(1,0,0),vec3(0.,0.,1.))
		bert:addPoint()
		cs2 = bert:endCrossSection()

		theta = 0
		theta2 = 0

		-- create a cylinder
		width = self.segWidth
		length = self.segLength

		bert:popState()

    	bert:setCrossSection(0)
		bert:setScale(width)
		bert:setStiffness(.3,.3)

		bert:beginCylinder()

		notScaled = true

	  	for i = 1, self.numSegs, 1 do
			width = randomN(self.segWidth, self.segWidth * .1)
			length = randomN(self.segLength, self.segLength * .1)

			pos = bert:getPosition()
			theta = self.meander * fracSum(pos.x, pos.y, pos.z, 3, 1.5)
			theta2 = self.meander2 * fracSum(pos.x, pos.y, pos.z, 1, 1)

 			bert:yaw(theta)
			bert:pitch(theta)
			bert:move(length)
			bert:addPoint(4)

        	bert:move(self.dl * .3)
    		bert:setCrossSection(cs1)
			bert:addPoint(1)

			bert:move(self.dl * .2)
    		bert:setCrossSection(cs2)
			bert:addPoint(1)

        	bert:move(self.dl * .2)
    		bert:setCrossSection(cs1)
			bert:addPoint(1)

        	bert:move(self.dl * .3)
    		bert:setCrossSection(0)
			bert:addPoint(1)

--			length = length * .9
		end

        bert:setScale(self.segWidth)
		bert:move(self.dl)
		bert:addPoint(1)

        bert:setScale(0.1 * self.segWidth)
		bert:move(self.segLength * .5)
		bert:endCylinder(4)
	end

	return obj
end
