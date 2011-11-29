
local underscore_docs = {
	map=[[map(iter, func) Produces a new array by mapping each value in iter through a transformation function.]],
	each=[[each(iter, func)  Passes each value to function(i) and returns the input.]],
	select=[[select(iter, func) Removes items that do not match the provided criteria.]],
	reject=[[reject(iter, func) Removes items that match the provided criteria.]],
	invoke=[[invoke(list_of_items, function_name) Calls a function with specified name on each item using the colon operator.]],
	pluck=[[pluck(iter, property_name) An convenient version of the common use-case of mapextracting a list of properties.]],
	reduce=[[reduce(iter, memo, func) Boils down a list of values into a single value. Memo is the initial state of the reduction, and each successive step of it should be returned by func.]],
	detect=[[detect(iter, func) Looks through a list returning the first element that matches a truth function. The function returns as soon as it finds an acceptable element.]],
	all=[[all(iter, [func]) Returns true if func(item) returns true for all item in items.]],
	any=[[any(iter, [func]) Returns true if func(item) returns true for any item in items.]],
	to_array=[[to_array(iter) Collects all values into a new array.]],
	sort=[[sort(iter, [comparison_func]), Returns an array with all elements sorted by the comparison function, by default it uses the < operator. If an array is passed in it will sort the elements in place and not create a new array.]],
	reverse=[[reverse(iter, [comparison_func]) Iterates over all the items and returns a new array with the items in reverse order.]],
	flatten=[[flatten(array) Flattens a nested array (the nesting can be to any depth).]],
	first=[[first(array, [length]) Returns the first element of an array. Optionally it will return an array of the first n items.]],
	rest=[[rest(array, [start_index]) Returns an array of all items except the first. Optionally it will start at start_index.]],
	slice=[[slice(array, start_index, length) Returns a section of an array starting with start_index of length items length.]],
	push=[[push(array, item) Inserts item at the end of an array]],
	pop=[[pop(array) Removes and returns the last item in an array]],
	shift=[[shift(array) Removes and returns the first item in an array]],
	unshift=[[unshift(array, item) Inserts item at the beginning of an array]],
	join=[[join(array) Returns a string with all items in the array concatenated together with an optional separator.]],
	extend=[[extend(destination, source) Copy all of the properties in the source object over to the destination object.]],
	keys=[[keys(object) Returns an array of all the property names in a table. (Note: order is not defined)]],
	values=[[values(object) Returns an array of all the property values in a table. (Note: order is not defined)]],
	is_empty=[[is_empty(object) Returns true if object contains no values.]],
	curry=[[curry(func, arg) Creates a wrapper function substituing the supplied arg for the first argument in original function.]],
	wrap=[[wrap(func, wrapper) Wraps the first function inside of the wrapper function, passing it as the first argument. This allows the wrapper to execute code before and after the function runs, adjust the arguments, and execute it conditionally.]],
	compose=[[compose(func1, func2, [...]) Returns the composition of a list of functions, where each function consumes the return value of the function that follows. In math terms, composing the functions f(), g(), and h() produces f(g(h())).]]
}
	
-- add the underscore library to util
__.each(__.functions(), function(f)
	-- print(f, __[f])
	if not set {"max","min"}[f] then
		local fn = f
		-- set_function_name(_G[f],fn)
		if underscore_docs[fn] then
			document(underscore_docs[fn] .. " (see http://mirven.github.com/underscore.lua/ for more info.)")(_G[f])
		else
			document(fn .. " is documented externally at http://mirven.github.com/underscore.lua/")(_G[f])  
		end
		categorise(_G[f],"util")
	end
end)
	
