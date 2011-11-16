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
require "core.node"
require "core.geometry"
require "core.turtle"
-- include underscore into global namespace
__ = require "core.underscore"
__.each(__.functions(), 
	function(f) 
		_G[f] = __[f]
	end
)
require "core.underscore_docs"
require "core.ui"