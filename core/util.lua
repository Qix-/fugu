--[[
-- Miscellaneous helper things
--]]

require "table"
require "core.help"
require "math"

-- put foreach into the global namespace
concat = table.concat
foreach = table.foreach
insert = table.insert
maxn = table.maxn
remove = table.remove
sort = table.sort
-- find = table.find

document"concat(table [,sep [,i [,j]]]) returns table[i]..sep..table[i+1].. ..sep..table[j]"(concat)
document"foreach(table, f) executes f on each element of table"(foreach)
document"insert(table, [pos,] value) inserts value at position pos in table"(insert)
document"maxn(table) returns the largest positive numerical index of table"(maxn)
document[[remove(table [,pos]) 
	removes the element at position pos from table, shifting down other elements to 
	close the space. returns the removed element. pos defaults to #table.]](remove)
document[[sort(table [,comp])
	sorts table elements in a given order, in-place, from table[1] to table[n]
	comp is an optional comparison function s.t., comp(a,b) = true if a<b.]](sort)
-- document[[find(table, f) returns an element e in table satisfying f(e)]](find)

categorise(concat,"util")
categorise(foreach,"util")
categorise(insert,"util")
categorise(maxn,"util")
categorise(remove,"util")
categorise(sort,"util")
--categorise(find,"util")

-- the following two functions are from:
-- http://stackoverflow.com/questions/656199/search-for-an-item-in-a-lua-list
function set(t) 
  local u = { }
  for _, v in ipairs(t) do u[v] = true end
  return u
end
document[[set(l) converts list l to a set]](set)
categorise(set,"util")

function find(l, f) 
  for i, v in ipairs(l) do
    if f(v) then
      return i, v
    end
  end
  return nil, nil
end
document[[find(l,f) finds element v of l satisfying f(v)]](find)
categorise(find,"util")

-- deepcopy from http://lua-users.org/wiki/CopyTable
function copy(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local new_table = {}
        lookup_table[object] = new_table
        for index, value in pairs(object) do
            new_table[_copy(index)] = _copy(value)
        end
        return setmetatable(new_table, getmetatable(object))
    end
    return _copy(object)
end
document[[copy(table) returns a (deep) copy of a table]](copy)
categorise(copy,"util")



-- printing functions (http://lua-users.org/wiki/TableSerialization)
function table_print (tt, indent, done)
  done = done or {}
  indent = indent or 0
  if type(tt) == "table" then
    local sb = {}
    for key, value in pairs (tt) do
      table.insert(sb, string.rep (" ", indent)) -- indent it
      if type (value) == "table" and not done [value] then
        done [value] = true
        table.insert(sb, "{\n");
        table.insert(sb, table_print (value, indent + 2, done))
        table.insert(sb, string.rep (" ", indent)) -- indent it
        table.insert(sb, "}\n");
      elseif "number" == type(key) then
        table.insert(sb, string.format("\"%s\"\n", tostring(value)))
      else
        table.insert(sb, string.format(
            "%s = \"%s\"\n", tostring (key), tostring(value)))
       end
    end
    return table.concat(sb)
  else
    return tt .. "\n"
  end
end

function to_string( tbl )
    if  "nil"       == type( tbl ) then
        return tostring(nil)
    elseif  "table" == type( tbl ) then
        return table_print(tbl)
    elseif  "string" == type( tbl ) then
        return tbl
    else
        return tostring(tbl)
    end
end
document[[to_string(tbl) converts tbl to a human-readable string]](to_string)
categorise(to_string,"util")


function choose(list)
	return list[math.floor(random(1,1+#list))]
end
document[[choose(list) returns a random element from list]](choose)
categorise(choose, "util")
