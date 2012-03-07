module(...,package.seeall)

local m = {}
local ls
local n = {}
local bert
local array_concat
local cs = {}

local AXIAL_DETAIL = 2
local SPEED_UP = 30 -- 10 --  100
-- print = function(...) end

PHI = 2.39996 -- *.3
STEM_SEGS = 1
NUM_THINGS = 30 -- 40
LENGTH_OF_THINGY = .8
LENGTH_SCALE = .9
INTER_THING_LENGTH = .2
TURN_OUTWARD = .4
TRUNK_CURVE = 0 -- 0.1
THINGY_SCALE = .5 -- .8
BULBY_SCALE = 1

local t = 0
local c = 0

function setup()
	t = 0
	c = 0
	
	-- add_slider {var="LENGTH_SCALE", value=LENGTH_SCALE, low=0.8, high=2}
	
	axiom = '^(-pi/2) G#(1) (Gsc(.2),0) Gs G#(0) (A(.3,.2,STEM_SEGS,TRUNK_CURVE),0) Ge(4)'
	rules = {
			'(A(l,w,n,b),1) : n > 0  -> (S(l,b,w),0)(A(l,w*.9,n-1,b-.02),0)',
			'(A(l,w,n,b),1) : n == 0 -> (B(INTER_THING_LENGTH,.15,NUM_THINGS,.0),0)',
			
			'(B(l,w,n,b),1) : n > 0  -> [ \\(PHI * n) (Gsc(w*.9),0) Gs (C(l*LENGTH_OF_THINGY*random(.9,1.5),w*.8,9,.1),0) Ge(4) ] (S(l,b,w),0) (B(l*LENGTH_SCALE,w*.9,n-1,b),0)', 
			
			'(C(l,w,n,b),1) : n > 0  -> (S(THINGY_SCALE*l,b,1.2*w),0) (C(l,w*.99,n-1,b-.028),0)',
			-- '(C(l,w,n,b),1) : n == 0 -> (S(.5*l,.5*TURN_OUTWARD,.9*l),0) (S(.5*l,.5*TURN_OUTWARD,.36*l),0) (S(.55*l,TURN_OUTWARD,.37*l),0) (S(.2*l,0,.46*l),0) (f(.15*l),0) (Gsc(.36*l),0)',
			
			'(C(l,w,n,b),1) : n == 0 -> (S(.5*l,.5*TURN_OUTWARD,.9*l),0) (S(.5*l,.5*TURN_OUTWARD,.75*l),0) (S(.55*l,TURN_OUTWARD,.6*l),0) (S(.55*l,TURN_OUTWARD,.6*l),0) (S(.4*l,0,.5*l),0) (S(.5*l,0,.46*l),0) (f(.25*l),0) (Gsc(.6*l),0)',
			
			'(S(l,b,w),15)  : true   -> S(l,b,w)'
			--'A(x,y)   : y < 9 -> Gs (f(x),0) Ge(5)',
			--'(f(x),5) : true  -> f(x)'
			--'(A(x,y),3) : y < 7 -> Gs (f(x),0) Gc(4) [ -(1) (A(x*.7,y+3),0) ] [ +(1) (A(x*.7,y+3),0) ] (f(x),0) Ge(4) ^(math.pi/2) (A(x*.8,y+1),0)',
			--'(f(x),5) : true -> (f(x),5)'
	}
	
	ls = parse_lsystem(axiom,rules)
		
	bert = fg.turtle()
	cs[1] = new_bumpy_cs(5, 1.5, .2, 0, 1)
	cs[1]:init()
	bert:pushState()
  cs[1]:build(bert)
	bert:popState()

	bert:setCarrierMode(0)
  ls:interp(bert)
	m = bert:getMesh()
	n = fg.meshnode(m)
	fgu:add(n)
end


function update(dt)
  local dt = SPEED_UP*dt
	t = t + dt
	if c < 1000 then
      ls:derive(dt)
			
			if (c%50==0) then
				bert = fg.turtle()
				cs[1]:update(dt)
				bert:pushState()
				cs[1]:build(bert)
				bert:popState()
				bert:setCarrierMode(0)
				-- print(t)
        ls:interp(bert)
	      m = bert:getMesh()
	      n:set_mesh(m)
				
				local vt = vertexlist(m)
				local end_verts = {}
				for i=1,bert:getNumEndCaps() do
					end_verts[#end_verts+1] = vt[bert:getEndCap(i-1)+1]
				end
				
				each(end_verts, function(v)
					-- get the size of the neighbourhood
					local outer = loopv(v)
					local edge_length = 0
					each(outer, function(n)
						edge_length = distance(v.p,n.p)
					end)
					local avg_edge_length = edge_length / #outer
					extrude_and_scale(m,v,v.n,-1*avg_edge_length,.5) 
					extrude_and_scale(m,v,v.n,-2*avg_edge_length,.8)
					extrude_and_scale(m,v,v.n,-2*avg_edge_length,1.2) -- .2)
					-- extrude_and_scale(m,v,v.n,-.2,1.2) -- .2)		
				end)
      end
      c = c + 1			
	end

--  if c == 60 then
--      print(ls:string())
--      c = 0 
--  end
end

print = function(...) end


-- return a new array containing the concatenation of all of its 
-- parameters. Scaler parameters are included in place, and array 
-- parameters have their values shallow-copied to the final array.
-- Note that userdata and function values are treated as scalar.
array_concat = function(...) 
    local t = {}
		for n = 1,select("#",...) do
        local arg = select(n,...)
        if type(arg)=="table" then
           for _,v in ipairs(arg) do
                t[#t+1] = v
            end
        else
            t[#t+1] = arg
        end
    end
    return t
end

-- Given a symbol, parameter list and age returns a human readable string
function symbol_tostring(s,p,a)
    local str = ''
    if a >= -1 then
        str = '(' .. s

        if #p ~= 0 then 
            str = str .. '('

            for k,v in ipairs(p) do
                str = str .. v .. ','
            end
            str = str:sub(1,-2)
            str = str .. ')'
        end

        str = str .. ',' .. a .. ')'
    else
        str = s 
        
        if #p ~= 0 then 
            str = str .. '('

            for k,v in ipairs(p) do
                str = str .. v .. ','
            end
            str = str:sub(1,-2)
            str = str .. ')'
        end
    end

    return str
end

-- Given a symbol in the form '(sym(p1,p2...pn),a)' returns the symbol character/string
-- the parameters, the age and the index of the list closing bracket
function parse_symbol(str)
    print("\n\n" .. str)
    local state = 0
    local b = ''
    local s = ''
    local p = {}
    local a = -10 
    local i = 0
    local count = 1
    
    for c in str:gmatch(".") do
        i = i + 1
        -- start symbol
        if state == 0 then
            if c == '(' then
                print('timed')
                state = 2
            elseif c:find('%s') then
                print('White space')
            else
                print('un-timed')
                b = b .. c
                state = 1
            end
        -- Untimed symbol
        elseif state == 1 then
            -- No parameters
            if c:find('%s') then
                print('No parameters')
                print(b)
                s = b
                return s,p,a,i
            -- Paramters begin
            elseif c == '(' then
                print('Parameters')
                print(b)
                s = b
                b = ''
                state = 3
            else
                b = b .. c
            end
        -- parse parameters
        elseif state == 3 or state == 4 then
            b = b .. c
            if c == '(' then
                count = count + 1
            elseif c == ')' then
                count = count - 1
            elseif c == ',' and count == 1 then
                print('Parameter = ' .. b:sub(1,-2))
                p[#p+1] = b:sub(1,-2)
                b = ''
            end
            if count == 0 then
                --print(b:sub(1,-2))
                print('Parameter = ' .. b:sub(1,-2))
                p[#p+1] = b:sub(1,-2)
                b = ''
                if state == 3 then
                    return s,p,a,i
                else
                    state = 5
                end
            end
        -- Timed symbol
        elseif state == 2 then
            -- No parameters
            if c == "," then
                print('No parameters')
                print(b)
                s = b
                b = ''
                state = 6
            -- start of parameters
            elseif c == '(' then
                print('Parameters')
                print(b)
                s = b
                b = ''
                --print(s)
                --print('Has ps')
                state = 4
            else 
                b = b .. c
            end
        elseif state == 5 then
            if c == ',' then
                state = 6
            end
        -- Time
        elseif state == 6 then
            if c:find("[0-9]") then
                b = b .. c
            elseif c == ")" then
                print('Age: ' .. b)
                a = loadstring("return " .. b)()
                return s,p,a,i
            end
        end
    end

    -- Hit the end (i.e. un-timed no parameter symbol)
    print("Hit the end of str")
    print(b)
    s = b
    return s,p,a,i
end

function parse_symbol2(str)
    local s = ''
    local p = {}
    local a = nil 
    local state = 0
    local i = 1
    local j = 1

    while j <= #str do
        -- timed symbol
        if state == 0 and str:sub(j,j) == '(' then
            print('Timed')
            a = 0
            i = j + 1
            j = j + 1
            state = 1
        -- un-timed symbol
        elseif state == 0 and str:sub(j,j) ~= ' ' then
            print('Not timed')
            a = nil
            i = j
            j = j+1
            state = 1
        -- The symbol ends, parameters begin
        elseif state == 1 and str:sub(j,j) == '(' then
            s = str:sub(i,j-1)
            print(s)
            i = j + 1
            j = j + 1
            state = 2
        -- Timed symbol ends, no parameters
        elseif state == 1 and str:sub(j,j) == ',' then
            s = str:sub(i,j-1)
            print(s)
            i = j + 1
            j = j + 1
            p = {}
            state = 3
        -- Un timed symbol ends, no parameters
        elseif state == 1 and str:sub(j,j) == ' ' then
            s = str:sub(i,j-1)
            print(s)
            i = j + 1
            j = j + 1
            p = {}
            state = 3
        -- The symbol
        elseif state == 1 then
            j = j + 1
        -- The parameters
        elseif state == 2 then
            local count = 1
            while count ~= 0 do
                if str:sub(j,j) == '(' then
                    count = count + 1
                elseif str:sub(j,j) == ')' then
                    count = count - 1
                elseif str:sub(j,j) == ',' and count == 1 then
                    p[#p+1] = str:sub(i,j-1)
                    i = j + 1
                end
                j = j + 1
            end
            p[#p+1] = str:sub(i,j - 2)
            i = j + 1
            j = j + 1
            state = 3

            for k,v in ipairs(p) do
                print(v)
            end
        -- The age
        elseif state == 3 and not a then
            j = j - 2
            break
        elseif state == 3 and str:sub(j,j) ~= ')' then
            j =  j + 1
        elseif state == 3 and str:sub(j,j) == ')' then
            a = loadstring('return ' .. str:sub(i,j-1))()
            break
        else
            j = j + 1
        end
    end

    return s,p,a,j
end

-- Given a string containing one or more symbols returns all the symbol char/strings, parameters and ages
function parse_symbol_string(str)
    local s = {}
    local p = {}
    local a = {}
    local i = 0

    while i < #str do
        str = str:sub(i+1)
        s[#s+1],p[#p+1],a[#a+1],i = parse_symbol(str)
    end

    return s,p,a
end

-- (A(x,y,z),4) : x > y -> (B(x),0)
function parse_rule(str)
    print("\n\nRULE~~~")
    local s   = ''
    local p   = {}
    local b   = -10 
    local con = ''
    local suc = {}
    local sp  = {}
    local sa  = {}
    local j = 0

    -- The predecessor
    s,p,b,j = parse_symbol(str)
    print(symbol_tostring(s,p,b,j))
    str = str:sub(j+1)

    -- The condition
    j = 1
    for c in str:gmatch(".") do
        if c:match('%s') then
            j = j + 1
        elseif c == ':' then
            j = j + 1
            break
        end
    end
    str = str:sub(j)
    
    j = str:find('->')
    con = str:sub(1,j-1)
    str = str:sub(j+2)

    -- The result
    suc,sp,sa,j = parse_symbol_string(str)
    
    return s,p,b,con,suc,sp,sa
end

function parse_lsystem(axiom,rules)
   local s  = {}
   local p  = {}
   local a  = {}
   local rs = {}
   local j  = 0

   s,p,a,j = parse_symbol_string(axiom)

   for k,r in ipairs(rules) do
       rs[#rs+1] = new_rule(parse_rule(r))
       --print(rs[#rs]:tostring())
   end

   return new_lsys(s,p,a,rs)
end

function replace_list(alias,value,source)
    local result = {}

    for k,v in ipairs(source) do
        result[k] = " " .. source[k] .. " "
        for k2,v2 in ipairs(alias) do
          local tmp
          result[k],tmp = result[k]:gsub("([^%a_])" .. v2 .. "([^%a_])","%1" .. value[k2] .. "%2")
        end
        result[k] = loadstring("return " .. result[k]:sub(2,-2))()
    end

    return result
end

-- Returns a copy of source with all occurances of alias replaced with value.
-- Alias must be buffered by non-alphanumeric characters
function replace(alias,value,source)
    local result = " " .. source .. " "
    for k,v in ipairs(alias) do
      local tmp
      result,tmp = result:gsub("([^%a_])" .. v .. "([^%a_])","%1" .. value[k] .. "%2")
    end
    result = loadstring("return " .. result:sub(2,-2))()

    return tostring(result)
end

function new_lsys(syms,params,ages,rules)
   local obj = {
       s = syms,
       p = params,
       a = ages,
       r = rules,
       t = {}
   }
	 
	 -- grab the calling environment, to access the global params
	 local calling_env = getfenv()

   -- work out terminal ages
   for k,v in ipairs(obj.r) do
       obj.t[v.s] = v.b
   end

   obj.interp = function(self,turt)
			 setfenv(0, calling_env) -- set the environment to the calling scripts environment
       for k,v in ipairs(self.s) do
            if v == 'Gs' then
                turt:beginCylinder()
            elseif v == 'Ge' then
                turt:endCylinder(loadstring('return ' .. self.p[k][1])())
                --print(loadstring('return ' .. self.p[k][1])())
            elseif v == 'Gc' then
                turt:addPoint(loadstring('return ' .. self.p[k][1])())
						elseif v == 'Gsc' then
								local f = 1
								if self.t[v] ~= nil and self.a[k] >= -1 then
									f = smoothstep(0, self.t[v], self.a[k])
								end
								turt:setScale(loadstring('return ' .. self.p[k][1])()*f)
            elseif v == 'G#' then
                turt:setCrossSection(loadstring('return ' .. self.p[k][1])())
            elseif v == '[' then
                turt:pushState()
            elseif v == ']' then
                turt:popState()
            elseif v == '+' then
                turt:yaw(loadstring('return ' .. self.p[k][1])())
            elseif v == '-' then
                turt:yaw(loadstring('return -1 * ' .. self.p[k][1])())
            elseif v == 'f' then
                local a = 1
                if self.t[v] ~= nil and self.a[k] >= -1 then
	                a = smoothstep(0, self.t[v], self.a[k])
                end
                turt:move(loadstring('return ' .. self.p[k][1])()*a)
            elseif v == '^' then
                turt:pitch(loadstring('return ' .. self.p[k][1])())
						elseif v == '\\' then
								turt:roll(loadstring('return ' .. self.p[k][1])())
						elseif v == 'S' then
								local f = 1
								local f2= 1
								if self.t[v] ~= nil and self.a[k] >= -1 then
									f  = smoothstep(0, self.t[v], self.a[k] * 2)
									f2 = smoothstep(0, self.t[v], self.a[k])
								end
								
								if #self.p[k] == 1 then
									turt:move(loadstring('return ' .. self.p[k][1])()*f)
								elseif #self.p[k] == 2 then
									turt:pitch(loadstring('return ' .. self.p[k][2])()*f)
									turt:move(loadstring('return ' .. self.p[k][1])()*f)
									turt:pitch(loadstring('return ' .. self.p[k][2])()*f)
								elseif #self.p[k] == 3 then
									turt:pitch(loadstring('return ' .. self.p[k][2])()*f)
									turt:move(loadstring('return ' .. self.p[k][1])()*f)
									turt:pitch(loadstring('return ' .. self.p[k][2])()*f)
									turt:setScale(loadstring('return ' .. self.p[k][3])()*f2)
								elseif #self.p[k] == 4 then
									turt:pitch(loadstring('return ' .. self.p[k][2])()*f)
									turt:move(loadstring('return ' .. self.p[k][1])()*f)
									turt:pitch(loadstring('return ' .. self.p[k][2])()*f)
									turt:setScale(loadstring('return ' .. self.p[k][3])()*f2)
									turt:setCrossSection(loadstring('return ' .. self.p[k][4]))
							  end
								turt:addPoint(AXIAL_DETAIL)
            end
       end
   end

   obj.string = function(self)
       local s = ''

       for k,v in ipairs(self.s) do
           s = s .. symbol_tostring(v,self.p[k],self.a[k])
       end

       return s
   end

   obj.derive = function(self,dt)
      local ns = {}
      local np = {}
      local na = {}

      -- For each symbol
      for k,v in ipairs(self.s) do
          local s,p,a
          -- update the age
          if self.a[k] >= -1 then
            self.a[k] = self.a[k] + dt
          end
          -- replace if required
          s,p,a = self:replace(v,self.p[k],self.a[k])
          ns = array_concat(ns,s)
          np = array_concat(np,p)
          na = array_concat(na,a)
      end

      self.s = ns
      self.p = np
      self.a = na
   end

   obj.replace = function(self,s,p,a)
      -- check each rule
      for k,v in ipairs(self.r) do
          -- if it matches do an expansion
          if v:match(s,p,a) then
              return v:expand(p)
          end
      end

      return {s},{p},{a}
  end

  return obj
end

function new_rule(symbol,params,terminalAge,cond,successor,successorParams,successorAges)
    local obj = { 
       s   = symbol,
       p   = params,
       b   = terminalAge,
       c   = cond,
       suc = successor,
       sp  = successorParams,
       sa  = successorAges,
    }

    obj.tostring = function(self)
        local s = symbol_tostring(self.s,self.p,self.b)

        s = s .. ' : ' .. cond .. ' -> '

        for k,v in ipairs(self.suc) do
            s = s .. symbol_tostring(v,self.sp[k],self.sa[k])
        end

        return s
    end

    obj.match = function(self,s,p,a)
        -- Check symbols match
        if s ~= self.s then
            return false
        end

        -- Check they are both timed
        if a and self.b then
            -- Check it is old enough
            if a < self.b then
                return false
            end
        -- Check that neither is timed
        elseif a or self.b then
            return false
        end

        -- Check the correct number of parameters
        if #self.p ~= #p then
            return false
        end

        -- Check the condition is meet
        local c = replace(self.p,p,self.c)

        return loadstring("return " .. c)()
    end

    obj.expand = function(self,params)
        local s = {}
        local p = {}
        local a = {}
        -- replace variables with values
        for k,v in ipairs(self.suc) do
            s[k] = self.suc[k]
            p[k] = replace_list(self.p,params,self.sp[k])
            a[k] = self.sa[k]
        end

        return s,p,a
    end

    return obj
end

--function parse(str)
--    local state = 0
--    local b = ''
--    local s = ''
--    local a = nil
--
--    for c in str:gmatch(".") do
--        -- Look for start of symbol
--        if state == 0 then
--            -- ( means a timed symbol
--            if c == '(' then
--                state = 1
--                a = 0
--            -- anything else (except white space) is a non-timed symbol
--            elseif c:gmatch("[^%s]") then
--                state = 1
--                a = nil
--                b = b .. 'c'
--            end
--        -- Get the alpha-numeric representation of the character
--        if state == 1 then
--            -- ( means parameters begin
--            if c == '(' then
--                s = b
--                b = ''
--                state == 2
--            -- , or space means no parameters
--            elseif c == ',' or c:gmatch("%s") then
--                s = b
--                b = ''
--                state == 3
--        -- Get the parameters
--        if state == 2 then
--            local count = 1
--            while count ~= 0 do
--                b = b .. c
--                if c == '(' then
--                    count = count + 1
--                elseif c == ')' then
--                    count = count - 1
--                elseif c == ',' and count == 1 then
--                    p[#p+1] = b:sub(1,-2)
--                    b = '' 
--                end
--            end
--            p[#p+1] = b:sub(1,-2)
--            state = 5
--        end
--    end
--end

function new_bumpy_cs(nbumps,size,noise,initAge,growTime)
	obj = {}
  obj.age = initAge
  obj.gt = growTime
	obj.nbumps = nbumps
	obj.size = size
	obj.noise = noise

	obj.t = {}
	obj.s = {}
	
	obj.update = function(self, dt) 
    self.age = self.age + dt
  end 

	obj.init = function(self)
		theta = 0
		thetaInc = math.pi / self.nbumps

		i = 1
		while (i <= self.nbumps * 2) do
			-- non bump
			self.t[i] = randomN(theta, thetaInc * self.noise)
			self.s[i] = randomN(1, self.noise)

			i = i + 1
			theta = theta + thetaInc

			-- bump
			self.t[i] = randomN(theta, thetaInc * self.noise)
			self.s[i] = randomN(self.size, (self.size - 1) * self.noise)

			i = i + 1
			theta = theta + thetaInc
		end
	end

	obj.build = function(self,bert)
		--local a = (self.size - 1) * smoothstep(0, self.gt, self.age)

		-- first point (not a bump)
		y = self.s[1] * math.sin(self.t[1])
		x = self.s[1] * math.cos(self.t[1])

		local dy = math.cos(self.t[1])
		local dx = -math.sin(self.t[1])

		bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))

		bert:beginCrossSection()

		-- add a bump
		y = self.s[2] * math.sin(self.t[2])
		x = self.s[2] * math.cos(self.t[2])

		local dy = math.cos(self.t[2])
		local dx = -math.sin(self.t[2])

		bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))

		i = 3
		while (i <= self.nbumps * 2) do
			bert:addPoint()

			-- add the next non bump
			y = math.sin(self.t[i])
			x = math.cos(self.t[i])

			dy = math.cos(self.t[i])
			dx = -math.sin(self.t[i])

			bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))
			bert:addPoint()

			i = i + 1

			-- add the next bump
			y = self.s[i] * math.sin(self.t[i])
			x = self.s[i] * math.cos(self.t[i])

			dy = math.cos(self.t[i])
			dx = -math.sin(self.t[i])

			bert:setFrame(vec3(x,y,0),vec3(dx,dy,0),vec3(0,0,1))

			i = i + 1
		end

		return bert:endCrossSection()
	end

	return obj
end

