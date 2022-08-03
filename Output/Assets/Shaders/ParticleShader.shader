
#ifdef __Vertex_Shader__

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec4 inColor;
layout(location = 3) in mat4 modelMatrix;

out vec4 ourColor;
out vec2 TexCoord;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
    ourColor = inColor;
    TexCoord = texCoord;
}

#endif

//--------------------

#ifdef __Fragment_Shader__

in vec4 ourColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;

void main()
{
  
   color = texture(ourTexture, TexCoord)  * ourColor;

}

#endif


