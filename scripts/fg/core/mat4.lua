-- fg.core.mat4
module(...,package.seeall)

--[[

mat4 is a 4x4 matrix based on GLSL mat4

--]]

local mat4 = {}
mat4.mt = {__index=mat4}

-- mat4.new accepts elements in column-major order
function new (a,b,c,d, e,f,g,h, i,j,k,l, m,n,o,p)
	local m4 = {[0]=a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p}
	setmetatable(m4,mat4.mt)
	-- m4.__index = mat4
	return m4
end

-- set column i, row j
function mat4:set (i, j, el)
	self[i+j*4] = el
end

-- get column i, row j
function mat4:get (i, j)
	return self[i+j*4]
end

mat4.mt.__tostring = function(m)
	local s = ""
	for j=0,3 do -- rows
		for i=0,3 do -- cols
			s = s .. m:get(i,j) .. (j==3 and i==3 and " " or ",")
		end
	end
	return s
end

