--[[
-- Miscellaneous helper things
--]]

module(...,package.seeall)


-- the following two functions are from:
-- http://stackoverflow.com/questions/656199/search-for-an-item-in-a-lua-list

-- convert a list to a set
function table.set(t) 
  local u = { }
  for _, v in ipairs(t) do u[v] = true end
  return u
end

-- find element v of l satisfying f(v)
function table.find(l, f) 
  for i, v in ipairs(l) do
    if f(v) then
      return i, v
    end
  end
  return nil, nil
end