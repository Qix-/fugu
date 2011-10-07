#version 150 compatibility
#extension GL_EXT_geometry_shader4 : enable

/*
 * Shader from http://code.google.com/p/shader-artist
 */
 
 
out float LightIntensity;

const vec3  LightPos = vec3( 0.0, 10.0, 0.0 );
const int   Level    = 3;
const float Radius   = 3.0;

vec3 V0, V01, V02;

/*
void main(){
  for(int i = 0; i < gl_VerticesIn; ++i){
    gl_Position = gl_ModelViewProjectionMatrix*gl_PositionIn[i];
    LightIntensity = 1.0;
    EmitVertex();
  }
}

*/
void ProduceVertex( float s_, float t_ )
{
    vec3 vertex = normalize( V0 + s_*V01 + t_*V02 );
    vec3 Normal = normalize( gl_NormalMatrix * vertex );
    vec4 EyePos = gl_ModelViewMatrix * vec4( Radius*vertex, 1.0 );
    
    // For lighting
    // LightIntensity = 1.5*abs( dot( normalize( LightPos - EyePos.xyz ), Normal ) );
    LightIntensity = 1.0;
    
    gl_Position = gl_ProjectionMatrix * EyePos;
    // gl_Position = vec4(vertex,1); // glModelViewProjectionMatrix * vertex;
    EmitVertex();
}

void main()
{
    V01 = ( gl_in[ 1 ].gl_Position - gl_in[ 0 ].gl_Position ).xyz;
    V02 = ( gl_in[ 2 ].gl_Position - gl_in[ 0 ].gl_Position ).xyz;
    V0  = gl_in[ 0 ].gl_Position.xyz;
    
    int numLayers = 1 << Level;
    
    float dt    = 1.0 / float( numLayers );
    float t_top = 1.0;
    float t_bot = 1.0 - dt;
    
    for( int it = 0 ; it < numLayers ; ++it, t_top = t_bot, t_bot -= dt )
    {
        float smax_top = 1.0 - t_top;
        float smax_bot = 1.0 - t_bot;
        
        int nums = it + 1;
        float ds_top = smax_top / float( nums - 1 );
        float ds_bot = smax_bot / float( nums );
        
        float s_top = 0.0;
        float s_bot = 0.0;
        
        for( int is = 0 ; is < nums ; ++is, s_top += ds_top, s_bot += ds_bot )
        {
            ProduceVertex( s_bot, t_bot );
            ProduceVertex( s_top, t_top );
        }
        
        ProduceVertex( s_bot, t_bot );
        EndPrimitive();
    }
}

