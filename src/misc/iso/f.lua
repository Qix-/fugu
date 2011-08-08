function distsq(a,b)
	return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) + (a.z-b.z)*(a.z-b.z)
end

function distsqa(a,b)
	return (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]) + (a[3]-b[3])*(a[3]-b[3])
end

function sphere(x,y,z,t)
	return distsqa({x,y,z},{0,0,0}) - .5
end

function f1(x,y,z,t)
	local st = .4 + .2*math.sin(t)
	local de = 5
	local r2 = x*x + y*y + z*z
	return (r2 - .2) + 0.5*noise(st*x*de,st*y*de,st*z*de)
end

function f2(x,y,z,t)
	local st = .4 + .2*math.sin(t)
	local de = 5
	local r2 = x*x + y*y + z*z
	local th = math.atan2(z,x)
	return .2*math.cos(th) + (r2 - .2) + 0.5*noise(st*x*de,st*y*de,st*z*de)
end

function f3(x,y,z,t)
	t = t
	local st = .5 + .5*math.sin(t)
	local de = 5
	local r2 = x*x*(1+.2*math.sin(.77*t)) + y*y + z*z
	return (r2 - .2) + 0.5*noise(st*x*de,st*y*de,st*z*de)
end

function f4(x,y,z,t)
	local pe = .1
	local o = {0,0,0}
	local p = {x,y,z}
	local d = distsqa(p,o)
	d = d*1.5+.2*math.sin(noise(pe*t,pe*t,pe*t)*20*y + t)
	return d*f3(x,y,z,t)
end

function f5(x,y,z,t)
	local d = distsqa({x,y,z},{0,0,0})
	local pt = 5*t
	local q = 10

	local _x = math.sin(q*x)
	local _y = math.sin(q*noise(t*x,t*y,t*z))
	local _z = math.sin(q*z)

	d = d*1.7 + .1*(_x+_y+_z)
	return d - .5
end

function zero(x,y,z,t)
	return {0,0,0}
end

function ggg(x,y,z,t)
	local p = 1
	local _t = 1
	local _ = .3 + .1*math.sin(2*t)
	local n = _*noise(_t*t,p*y,p*z)
	local n2 = _*noise(p*x,_t*t,p*z)
	local n3 = _*noise(p*x,p*y,_t*t)
	return {n,n2,n3}
end

f = f4
g = ggg

-- iso = new Iso(function (x,y,z,t) return x*x end)

--[[
function f(x,y,z,t)
	local p1 = {x=x,y=y,z=z}
	local p2 = {x=0,y=.6*math.sin(t),z=0}
	local p3 = {x=.4*math.cos(.7*t),y=0,z=.4*math.sin(t)}

	local min = math.min(distsq(p1,p2),1.5*distsq(p1,p3))
	return min - .2
end
]]--
