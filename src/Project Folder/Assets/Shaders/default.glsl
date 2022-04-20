#ifdef vertex
#version 330 core

layout (location = 0) in vec3 meshVertices; //Let's go for all vertices for now
layout (location = 1) in vec3 normals;
//layout (location = 1) in int meshIndices;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 position;
uniform mat3 normalMatrix;

out vec3 Normal;
out vec3 FragPos;  

void main()
{
 mat4 offset = mat4(1.0, 0.0, 0.0, 0.0,
                   0.0, 1.0, 0.0, 0.0,
                   0.0, 0.0, 1.0, 0.0,
                   position.x, position.y, position.z, 1.0);

	Normal = normalMatrix * normals;
	FragPos = vec3(offset * vec4(meshVertices, 1.0f));

	gl_Position = projection * view * offset * vec4(meshVertices, 1.0f);
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

vec3 lightPos = vec3(5.0, 5.0, 0.0); 
vec3 lightColor = vec3(1.0, 1.0, 1.0);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * color.xyz;

	FragColor = vec4(result, 1.0);
}
#endif

