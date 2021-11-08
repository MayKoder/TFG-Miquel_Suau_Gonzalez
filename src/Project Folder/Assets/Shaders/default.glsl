#ifdef vertex
#version 330 core

layout (location = 0) in vec3 meshVertices; //Let's go for all vertices for now
//layout (location = 1) in int meshIndices;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 position;

void main()
{
 mat4 offset = mat4(1.0, 0.0, 0.0, 0.0,
                   0.0, 1.0, 0.0, 0.0,
                   0.0, 0.0, 1.0, 0.0,
                   position.x, position.y, position.z, 1.0); 

	gl_Position = projection * view * offset * vec4(meshVertices, 1.0f);
}
#endif

#ifdef fragment
#version 330 core

void main()
{
	gl_FragColor = vec4(0.8, 0.8, 0.8, 1.0);
}
#endif

