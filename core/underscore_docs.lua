
-- add the underscore library to util
__.each(__.functions(), function(f)
	-- print(f, __[f])
	if not set {"max","min"}[f] then
		local fn = f
		-- set_function_name(_G[f],fn)
		document(fn .. " The underscore functions are documented at http://mirven.github.com/underscore.lua/")(_G[f])  
		categorise(_G[f],"utils (underscore)")
	end
end)
		