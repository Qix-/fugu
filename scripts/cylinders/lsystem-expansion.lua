
module(...,package.seeall)

-- num expansions
n = 4
-- rotation angle
delta = math.pi * .5
-- step length
d = 10

local m = {}
local array = {}
local node = {}
function setup()
	-- axiom
	--array[1] = new_F()
	--array[2] = new_minus()
	--array[3] = new_F()
	--array[4] = new_minus()
	--array[5] = new_F()
	--array[6] = new_minus()
	--array[7] = new_F()

	array[1] = new_minus()
	array[2] = new_F()

  	print('Axiom = ' .. arrayToString(array))

	--Do the expansions
  	for i=1,n do
		tmp = {}
		k = 0

		for i,v in ipairs(array) do
       	    for j,v2 in ipairs(v:expand()) do
				tmp[j + k] = v2
			end
			k = k + table.getn(v:expand())
		end

		array = tmp

	  	print("Expansion " .. i .. " = " .. arrayToString(array))
	end

	-- Interperate the result
	bert = fg.turtle()
	for i,v in ipairs(array) do
		v:interp(bert)
	end

	m = bert:getMesh()
	node = fg.meshnode(m)
	fgu:add(node)
end

local t = 0
function update(dt)
	t = t + dt
end

function arrayToString(arr)
    outS = ''
	for i,v in ipairs(arr) do 
		outS = outS .. arr[i]:toString() 
	end
	return outS
end

function copyArray(a1)
	a2 = {}
    for i,v in pairs(a1) do
		a2[i] = v
	end
	return a2
end

function new_F()
	local obj = {
	}

	obj.expand = function(self)
	  retA = {}
--	  retA[1] = new_F()
--	  retA[2] = new_plus()
--	  retA[3] = new_F()
--	  retA[4] = new_F()
--	  retA[5] = new_minus()
--	  retA[6] = new_F()
--	  retA[7] = new_F()
--	  retA[8] = new_minus()
--	  retA[9] = new_F()
--	  retA[10] = new_minus()
--	  retA[11] = new_F()
--	  retA[12] = new_plus()
--	  retA[13] = new_F()
--	  retA[14] = new_F()
--	  retA[15] = new_minus()
--	  retA[16] = new_F()
--	  retA[17] = new_minus()
--	  retA[18] = new_F()
--	  retA[19] = new_plus()
--	  retA[20] = new_F()
--	  retA[21] = new_plus()
--	  retA[22] = new_F()
--	  retA[23] = new_F()
--	  retA[24] = new_plus()
--	  retA[25] = new_F()
--	  retA[26] = new_F()
--	  retA[27] = new_minus()
--	  retA[28] = new_F()

	  retA[1] = new_F()
	  retA[2] = new_plus()
	  retA[3] = new_F()
	  retA[4] = new_minus()
	  retA[5] = new_F()
	  retA[6] = new_minus()
	  retA[7] = new_F()
	  retA[8] = new_plus()
	  retA[9] = new_F()
	  return retA;
	end

	obj.toString = function(self)
	  return 'F'
	end

	obj.interp = function(self,turtle)
	  bert:beginCylinder()
	  bert:move(d)
	  bert:endCylinder(1)
	end

	return obj
end

function new_plus()
	local obj = {
	}

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_plus()
	  return retA;
	end

	obj.toString = function(self)
	  return '+'
	end

	obj.interp = function(self,turtle)
	  bert:yaw(-delta)
	end

	return obj
end

function new_minus()
	local obj = {
	}

	obj.expand = function(self)
	  retA = {}
	  retA[1] = new_minus()
	  return retA;
	end

	obj.toString = function(self)
	  return '-'
	end

	obj.interp = function(self,turtle)
	  bert:yaw(delta)
	end

	return obj
end
