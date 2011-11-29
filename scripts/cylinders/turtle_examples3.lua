
module(...,package.seeall)

local m = {}
function setup()
    local length = 20
    local width = 1
    local its = 5
    local n = 5

	bert = fg.turtle()

    makeBranch(length,width,its,bert)

	m = bert:getMesh()
	fgu:addMesh(m)
end

local t = 0
function update(dt)
	t = t + dt
end

function makeBranch(length,width,rec,bert)
    if rec > 0 then
        bert:setScale(width)
        bert:yaw(math.pi*.1)
        bert:beginCylinder()
        bert:move(length*.5)

        bert:yaw(-math.pi*.1)
        bert:addPoint(7)
        bert:yaw(-math.pi*.1)

        bert:pushState()
        bert:yaw(math.pi*.5)
        makeBranch(length*.5,width*.5,rec-1,bert)
        bert:popState()

        bert:pushState()
        bert:yaw(-math.pi*.5)
        makeBranch(length*.5,width*.5,rec-1,bert)
        bert:popState()

        bert:pushState()
        bert:pitch(math.pi*.5)
        makeBranch(length*.5,width*.5,rec-1,bert)
        bert:popState()

        bert:pushState()
        bert:pitch(-math.pi*.5)
        makeBranch(length*.5,width*.5,rec-1,bert)
        bert:popState()

        bert:move(length*.5)
        --bert:yaw(math.pi*.1)
        bert:setScale(width*0.1)
        bert:endCylinder(7)
    end
end
