#ifdef vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vertColor;

out vec3 TexCoords;
out vec3 color;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    color = vertColor;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  
#endif

#ifdef fragment
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
in vec3 color;

uniform samplerCube skybox;

void main()
{    
    //FragColor = texture(skybox, TexCoords);
    FragColor = vec4(color.x, color.y, color.z, 1.0);
}
#endif

