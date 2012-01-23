// scales the vert pos relative to camera, for drawing a wireframe over top

void main() {					
	vec4 v = gl_ModelViewMatrix * gl_Vertex;
	v.xyz = v.xyz * 0.99;
	gl_Position = gl_ProjectionMatrix * v;
	gl_FrontColor = gl_Color;
}
