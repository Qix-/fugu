module(...,package.seeall)

local m = {}
local node = {}

function setup()

	b = new_cylax(.5,1.1,5)

    donatello = fg.turtle()
	donatello:pitch(-math.pi*.5)

	b:build(donatello)

	node = fg.meshnode(donatello:getMesh())
	fgu:add(node)

end

local t = 0
function update(dt)

end

function new_cylax(width,length,n)
	local obj = {
		baseWidth = width,
		length = length,
		numBumps = n,

		outerS = 1.8,
		innerS = 1.,
		innerRat = .7
	}

	obj.build = function(self, bert)
	    bert:pushState()
		
		-- create cross section
		bert:setFrame(vec3(1,0.,0.),vec3(0.,1.,0.),vec3(0.,0.,1.))
		bert:setStiffness(self.outerS,self.outerS)
		bert:beginCrossSection()
		y = self.innerRat * math.sin((.5) * 2 * math.pi / self.numBumps)
		x = self.innerRat * math.cos((.5) * 2 * math.pi / self.numBumps)

		dy = math.cos((.5) * 2 * math.pi / self.numBumps)
		dx = -math.sin((.5) * 2 * math.pi / self.numBumps)

		bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))
		bert:setStiffness(self.innerS,self.innerS)
		bert:addPoint()

    	i = 1
		while (i < self.numBumps) do
			y = math.sin(i * 2 * math.pi / self.numBumps)
			x = math.cos(i * 2 * math.pi / self.numBumps)

			dy = math.cos(i * 2 * math.pi / self.numBumps)
			dx = -math.sin(i * 2 * math.pi / self.numBumps)

			bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))
			bert:setStiffness(self.outerS,self.outerS)
			bert:addPoint()

			y = self.innerRat * math.sin((i+.5) * 2 * math.pi / self.numBumps)
			x = self.innerRat * math.cos((i+.5) * 2 * math.pi / self.numBumps)

			dy = math.cos((i+.5) * 2 * math.pi / self.numBumps)
			dx = -math.sin((i+.5) * 2 * math.pi / self.numBumps)

			bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))
			bert:setStiffness(self.innerS,self.innerS)
			bert:addPoint()

			 i = i + 1
		end

		cs = bert:endCrossSection()

		-- create a cylinder
		bert:popState()
		bert:pushState()

    	bert:setCrossSection(0)
		bert:setScale(self.baseWidth)

		bert:beginCylinder()
		bert:move(.5*self.length)
		bert:setScale(4.*self.baseWidth)
		bert:addPoint(3)
		bert:move(self.length)
    	bert:setCrossSection(cs)
		bert:setScale(5.*self.baseWidth)
		bert:addPoint(3)
		bert:move(self.length*2.4)
		bert:setScale(2.*self.baseWidth)
		bert:setCrossSection(0)
		bert:addPoint(8)
		bert:move(.8*self.length)
--		bert:addPoint(7)
--		bert:move(0.3*self.length)
		bert:setScale(.2*self.baseWidth)
		bert:endCylinder(3)

		bert:popState()

		bert:move(1.5*self.length)

--	  	for i = 1, self.numBumps, 1 do
--			bert:pushState()
--
--			brachioles = new_brachioles_bunch(self.baseWidth*.15,self.baseWidth*1.2,7,12)
--
--			pos = vec3(2.4*self.innerRat*math.cos((i + .5) * 2 * math.pi / self.numBumps), 
--			              2.4*self.innerRat*math.sin((i + .5) * 2 * math.pi / self.numBumps),
--						  0)
--			up = vec3(-math.cos((i+.5) * 2 * math.pi / self.numBumps),
--						   -math.sin((i+.5) * 2 * math.pi / self.numBumps),
--						   0)
--
--			donatello:setFrameRel( pos, vec3(0,0,1), up )
--
--			brachioles:build(donatello)
--
--			bert:popState()
--		end
--
	end

	return obj
end

