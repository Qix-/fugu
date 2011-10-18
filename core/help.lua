--	A help system based on: http://lua-users.org/wiki/DecoratorsAndDocstrings
--	Usage: 
--	To print the help for something:
--		help(some_function) 
-- 	To add a help string for a function, foo, execute:
--		document[[Foo does something cool]](foo)

require "table"

local docstrings = setmetatable({}, {__mode="kv"})
local categories = {}
local reverse_category_lookup = {}

function document(str)
	return function(obj)		
		docstrings[obj]=str; 
		return obj 
	end
end 

function help(obj)
	if (obj==nil) then	
		print("You can get help for some functions, f, by typing the command \"help(f)\" Type \"show_commands()\" to get a list of all _documented_ functions.")
	elseif (docstrings[obj]) then
		print(docstrings[obj])
	else
		print("No help exists for this object.")
	end
end

document[[Print the documentation for a given object. Assign documentation with the document function.]](help)
document[=[Add a string as documentation for an object. For example:
	document[[foo does something awesome.]](foo), 
	or during function specification with
	f = document[[Print a hello message]](
		function()
			print("hello")
		end
	)]=](document)
	
function show_commands()
	print("All documented commands")
	foreach(_G, function(k,v) 
		if docstrings[v] then print(k,docstrings[v]) end
		end)
end
document[[Show all documented commands]](show_commands)

function categorise(f,cat)
	if (categories[cat]==nil) then
		categories[cat] = {}
	end
	table.insert(categories[cat],f)
	reverse_category_lookup[f] = cat
end
document[[categorise(f,cat:string) puts f into category cat]](categorise)

function get_category(f)
	if (f==nil) then return nil
	else return reverse_category_lookup[f] end
end
document[[get_category(f) returns the category that f is in]](get_category)

function category_list(cat)
	if (cat==nil) then return nil
	else return categories[cat] end
end
document[[category_list(cat) returns a list of all functions in the specified category]](category_list)

function function_name(f)
	for k,v in pairs(_G) do
		if (f==v) then return k end
	end
end

function command_list_by_category()
	local list = {}
	foreach(categories, function(cat,val)
		foreach(val, function(_,f)
			list[#list+1] = {category=cat,name=function_name(f),docstring=docstrings[f]}
		end)
	end)
	return list
end

-- finally categorise help and show_commands
table.foreach({help,show_commands}, function(_,f) categorise(f,"help") end)
