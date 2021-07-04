#ifdef vertex
#version 330 core
layout (location = 0) in vec2 vertex;
layout (location = 0) in vec2 texCoord;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = texCoord;
    gl_Position = view * vec4(vertex.xy, 0.0, 1.0);
}  
#endif

#ifdef fragment
#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

void main()
{    
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
#endif
