#version 150

/*
 * Shader from http://code.google.com/p/shader-artist
 */
 

in float LightIntensity;

// out vec4 gl_FragColor;

void main()
{
    gl_FragColor = vec4( LightIntensity, 0.0, 0.0, 1.0 );    
}
