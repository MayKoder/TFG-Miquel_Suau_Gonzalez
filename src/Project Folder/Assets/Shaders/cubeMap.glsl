#ifdef vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vertColor;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  
#endif

#ifdef fragment
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 colorTint;

void main()
{    
    //FragColor = texture(skybox, TexCoords);
    FragColor = texture(skybox, TexCoords) * vec4(colorTint, 1.0);
}
#endif

