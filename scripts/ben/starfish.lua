module(...,package.seeall)

local m = {}
local node = {}

-- model parameters
p1 = 2
p2 = .3
p3 = 14
p4 = 0
PRECISION = 5

function setup()
	node = meshnode(make(p1,p2,p3,p4))
	fgu:add(node)
	
	add_slider {var="p1", value=p1, low=1, high=3}
	add_slider {var="p2", value=p2, low=0, high=1}
	add_slider {var="p3", value=p3, low=5, high=30}
end

function update(dt) 
	p4 = sin(fgu.t*pi + pi/2)
	-- p1 = 1 + fgu.t
	node:set_mesh(make(p1,p2,p3,p4))
end

function make(a,b,c,d)
	local creature = new_creature(a,b,c,d)

	local t = turtle()
	t:push()
	t:pitch(pi/2)
	creature:build(t)
	t:pop()

	return t:getMesh()
end

function new_tentacle(length,width,meander)
	local obj = {
		rootLength  = length,
		rootWidth   = width,
		rootMeander = meander,
		stepLength  = length / ceil(PRECISION/2)
	}

	obj.build = function(self,t)
		t:setScale(self.rootWidth)
		t:setStiffness(0.3,0.3)
		t:setCarrierMode(0)
		t:beginCylinder()
		dist = 0
		rateCur = 0
		stepLength = self.stepLength		
		t:move(self.stepLength)
		dist = dist + stepLength
		
		while (dist < self.rootLength) do			
			t:yaw(rateCur/2)
			t:addPoint(ceil(PRECISION/4))
			pos = t:getPosition()
			rateCur = rateCur + self.rootMeander
			t:yaw(rateCur/2)
			t:move(stepLength)
			dist = dist + self.stepLength
		end
		t:setScale(0.001)
		t:yaw(rateCur/2)
		t:endCylinder(ceil(PRECISION/4))
	end

	return obj
end

function new_creature(length,width,n,meander)
	local obj = {
		meanLength  = length,
		rootWidth   = width,
		numRoots    = n,
		rootMeander = meander,
		roots = {}
	}

	local points = distribute_points_sphere(n)

	obj.build = function(self,t)
		for i = 1, self.numRoots do
			-- create a root at point i
			length = self.meanLength
			self.roots[i] = new_tentacle(length,self.rootWidth,meander)

			local p = points[i]
			local dir = normalise(p)
			local up = perp(dir)
			t:push()
				t:setFrame(p*.3,dir,up)
				self.roots[i]:build(t)
			t:push()
		end
	end
	return obj
end

