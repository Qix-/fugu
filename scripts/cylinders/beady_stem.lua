module(...,package.seeall)

local m = {}
local node = {}
local first = true
local stalk = {}

function setup()
	-- The stalk
   	stalk = new_stalk(1.,1.,.03,.7)
end

local t = 0
function update(dt)
	t = t + dt

	m = stalk:getMesh()
	if (first) then
		-- Node for the mesh
		node = fg.meshnode(m) -- blank transform node
		fgu:add(node)
		first = false
	end
	node:setMesh(m)

	stalk:update(dt)
end

function new_stalk(l,d,dl,dd) -- use the global mesh m
	local obj = {
		maxLength = l,
		maxWidth = d,
		bumpLength = dl,
		bumpDepth = dd,
		growTimeLength = 10,
		t = 0,

		baseWidthInit = .2,
		tipWidthInit = .1,
		growTimeBase = 15,
		growTimeTip = 20,

		currentLength = 1,
		baseWidth = .4,
		tipWidth = .2
	}

	obj.getMesh = function(self)
	  bert = fg.turtle()

      bert:pushState()
	  bert:setFrame(fg.vec3(.07,0.,0.),fg.vec3(0.,1.,0.),fg.vec3(0.,0.,1.))
	  bert:setStiffness(0.3,0.3)
	  bert:beginCrossSection()
	  bert:setFrame(fg.vec3(0.,.07,0.),fg.vec3(-1.,0.,0.),fg.vec3(0.,0.,1.))
	  bert:addPoint()
	  bert:setFrame(fg.vec3(-.07,0.,0.),fg.vec3(0.,-1.,0.),fg.vec3(0.,0.,1.))
	  bert:addPoint()
	  bert:setFrame(fg.vec3(.0,-.07,0.),fg.vec3(1.,0.,0.),fg.vec3(0.,0.,1.))
	  inner = bert:endCrossSection()

	  -- Create the cylinder
	  bert:popState()
	  cLength = 0.
	  bert:setScale(self.baseWidth)
	  bert:beginCylinder()
	  while (cLength < self.currentLength - self.bumpLength) do
		cLength = cLength + self.bumpLength
		bert:setCrossSection(inner)
	  	bert:move(self.bumpLength)
		bert:addPoint()

		cLength = cLength + self.bumpLength
		bert:setCrossSection(0)
	  	bert:move(self.bumpLength)
		bert:addPoint()
	  end
	  bert:move(self.currentLength - cLength + self.bumpLength)
	  bert:setScale(self.tipWidth)
	  bert:endCylinder()
	  return bert:getMesh(1,150)
	end

	obj.update = function(self,dt)
	    self.t = self.t + dt
		t = fg.clamp(self.t, 0., self.growTimeLength)
		x = self.t / self.growTimeLength
		self.currentLength = self.maxLength * fg.smoothstep(0., 1, x)

		t = fg.clamp(self.t, 0., self.growTimeBase)
		x = self.t / self.growTimeBase
		self.baseWidth = self.baseWidthInit + self.maxWidth * fg.smoothstep(0., 1., x)

		t = fg.clamp(self.t, 0., self.growTimeTip)
		x = self.t / self.growTimeTip
		self.tipWidth = self.tipWidthInit + self.maxWidth * fg.smoothstep(0., 1., x)
	end

	return obj
end
