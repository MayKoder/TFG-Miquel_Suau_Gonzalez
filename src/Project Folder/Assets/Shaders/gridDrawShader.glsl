#ifdef vertex
#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

uniform vec2 gridPosition;

void main()
{
mat4 offset = mat4(1.0, 0.0, 0.0, 0.0,
                  0.0, 1.0, 0.0, 0.0,
                  0.0, 0.0, 1.0, 0.0,
                  gridPosition.x, 0.0, gridPosition.y, 1.0); 

	gl_Position = projection * view * offset *  vec4(aPos, 1.0f);
}
#endif

#ifdef fragment
#version 330 core

void main()
{
	gl_FragColor = vec4(1.0);
}
#endif
