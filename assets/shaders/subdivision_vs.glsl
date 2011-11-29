#version 150 compatibility

/*
 * Shader from http://code.google.com/p/shader-artist
 */



void main()
{
    // gl_Position = ftransform(); //
    gl_Position = gl_Vertex; 
}
