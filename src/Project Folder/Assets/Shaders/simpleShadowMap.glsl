#ifdef vertex
#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 modelMatrix;

void main()
{
	gl_Position = lightSpaceMatrix * modelMatrix * vec4(aPos, 1.0);
}

#endif

#ifdef fragment
#version 330 core

void main()
{
	//gl_FragDepth = gl_FragCoord.z;
}

#endif
