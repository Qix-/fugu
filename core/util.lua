--[[
-- Miscellaneous helper things
--]]

require "table"
require "core.help"

-- put foreach into the global namespace
foreach = table.foreach
categorise(foreach,"util")

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