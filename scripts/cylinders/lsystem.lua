
module(...,package.seeall)

local m = {}
function setup()
	bert = fg.turtle()

	string = {'-', 'F', '+', 'F', '+', 'F', '-', '&', 'F', '^', '-', 'F', '+', '&', '&', 'F', '+', 'F', '+'}
	interpret(bert, string)

	m = bert:getMesh(10,10)
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt
end

local d=1.
local del=math.pi*.5
function interpret(turt,string)
	for i,v in ipairs(string) do
		if (v == 'F') then
			turt:beginCylinder()
			turt:move(d)
			turt:endCylinder()
		elseif (v == 'f') then
			turt:move(d)
		elseif (v == '+') then
			turt:yaw(-del)
		elseif (v == '-') then
			turt:yaw(del)
		elseif (v == '&') then
			turt:pitch(-del)
		elseif (v == '^') then
			turt:pitch(del)
		elseif (v == '/') then
			turt:roll(-del)
		elseif (v == '\\') then
			turt:roll(del)
		elseif (v == '|') then
			turt:yaw(math.pi)
		end
	end
end

