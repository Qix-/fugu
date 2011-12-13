--[[
	A library of fg routines
--]]

require "core.help"
require "core.util"
require "core.math"
require "core.transform"
require "core.nloop"
require "core.mesh"
require "core.pos"
require "core.universe"
require "core.geometry"
require "core.turtle"
-- include underscore into global namespace
__ = require "core.underscore"
__.each(__.functions(), 
	function(f)
		-- only import the functions not in this list 
		if not set {"max","min"}[f] then
			_G[f] = __[f]
		end
	end
)
require "core.underscore_docs"
require "core.ui"