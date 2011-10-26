--[[
-- Miscellaneous helper things
--]]

require "table"
require "core.help"

-- put foreach into the global namespace
concat = table.concat
foreach = table.foreach
insert = table.insert
maxn = table.maxn
remove = table.remove
sort = table.sort

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

categorise(concat,"util")
categorise(foreach,"util")
categorise(insert,"util")
categorise(maxn,"util")
categorise(remove,"util")
categorise(sort,"util")

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
function deepcopy(object)
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
document[[deepcopy(table) returns a deep copy of a table]](deepcopy)
categorise(deepcopy,"util")
