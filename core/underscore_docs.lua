
-- add the underscore library to util
__.each(__.functions(), function(f)
	-- print(f, __[f])
	local fn = f
	-- set_function_name(_G[f],fn)
	document(fn .. " (see http://mirven.github.com/underscore.lua/)")(_G[f])  
	categorise(_G[f],"util")
end)
		