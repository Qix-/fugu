

document[[line_exp_point_dist(p1,p2,q) calculates the distance from q to the explicit line (p1,p2)]](line_exp_point_dist)
document[[segment_point_dist(p1,p2,q) calculates the distance from q to the explicit line segment (p1,p2)]](segment_point_dist)

foreach({line_exp_point_dist,segment_point_dist}, 
	function(_,v) categorise(v,"geometry") end
)