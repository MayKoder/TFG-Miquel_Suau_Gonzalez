#ifdef vertex
#version 330 core

layout (location = 0) in vec3 meshPos; //Move once per vertex iteration
layout (location = 1) in vec2 instancePos; //Move once per instance iteration

uniform mat4 view;
uniform mat4 projection;

void main()
{
mat4 offset = mat4(1.0, 0.0, 0.0, 0.0,
                  0.0, 1.0, 0.0, 0.0,
                  0.0, 0.0, 1.0, 0.0,
                  instancePos.x, 0.0, instancePos.y, 1.0); 

	gl_Position = projection * view * offset *  vec4(meshPos, 1.0f);
}
#endif

#ifdef fragment
#version 330 core

void main()
{
	gl_FragColor = vec4(1.0);
}
#endif
