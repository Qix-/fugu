//
// Bui Tuong Phong shading model (per-pixel) 
// 
// by 
// Massimiliano Corsini
// Visual Computing Lab (2006)
//
// modified by Ben Porter 2011 

varying vec3 normal;
varying vec3 vpos;
varying vec3 vcol;

void main()
{	
	// vertex normal
	normal = normalize(gl_NormalMatrix * gl_Normal);
	
	// vertex position
	vpos = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	// vertex colour
	vcol = vec3(gl_Color);
	
	// vertex position
	gl_Position = ftransform();
}
