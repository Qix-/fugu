--[[
	docs (and placeholders) for the ui routines
--]]


if (add_slider==nil) then
	add_slider = function(var,params) end
end

document[[add_slider {...} adds a slider to the ui in fugu
	E.g., add_slider {var="x", value=1, low=0, high=10} creates a slider that 
	binds to the variable "x" in the active module's namespace.	 
]](add_slider)
categorise(add_slider,"ui")

