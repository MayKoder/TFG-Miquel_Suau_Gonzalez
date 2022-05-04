#ifdef vertex
#version 330 core

layout (location = 0) in vec3 meshVertices; //Let's go for all vertices for now
layout (location = 1) in vec3 normals;
//layout (location = 1) in int meshIndices;

uniform mat4 view;
uniform mat4 projection;
//uniform vec3 position;
uniform mat3 normalMatrix;

uniform mat4 modelMatrix;

out vec3 Normal;
out vec3 FragPos;  

void main()
{
	Normal = normalMatrix * normals;

	FragPos = vec3(modelMatrix * vec4(meshVertices, 1.0f));

	gl_Position = projection * view * modelMatrix * vec4(meshVertices, 1.0f);
}
#endif

#ifdef fragment
#version 330 core

uniform vec4 color;
uniform vec3 viewPos; 

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;


void main()
{




vec3 lightDir = vec3(1.0, 1.0, 0.0); 
float intensity = dot(lightDir, Normal);
vec3 finalColor = vec3(1.0, 1.0, 1.0);

    if (intensity > 0.4)
		finalColor *= 1.0;
	else if(intensity > 0.1)
		finalColor *= 0.5;
    else
		finalColor *= 0.20;



	FragColor = vec4(finalColor, 1.0);
}
#endif

