#ifdef vertex
#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * vec4(aPos, 1.0f);
}
#endif

#ifdef fragment
#version 330 core

void main()
{
	gl_FragColor = vec4(1.0);
}
#endif
