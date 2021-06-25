#ifdef vertex
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normals;
layout (location = 3) in vec3 tangents;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform float time;

uniform vec3 lightPos;

out vec3 fNormal;
out vec3 fPosition;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

// 2D Random
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}


void main()
{


    // Scale the coordinate system to see
    // some noise in action
    vec2 pos2 = vec2(texCoord*5.0);

    // Use the noise function
    float n = noise(vec2(position.x, position.z) * time * 0.05);

fNormal = mat3(transpose(inverse(model_matrix))) * normals;

vec4 pos = model_matrix * vec4(position, 1.0);
fPosition = pos.xyz;

vec3 test = position.xyz;
test.y = n;
gl_Position = projection * view * model_matrix * vec4(test, 1.0f);

}
#endif

#ifdef fragment
#version 330 core

in vec3 fNormal;
in vec3 fPosition;

vec2 blinnPhongDir(vec3 lightDir, float lightInt, float Ka, float Kd, float Ks, float shininess)
{
vec3 s = normalize(lightDir);
vec3 v = normalize(-fPosition);
vec3 n = normalize(fNormal);
vec3 h = normalize(v+s);
float diffuse = Ka + Kd * lightInt * max(0.0, dot(n, s));
float spec = Ks * pow(max(0.0, dot(n,h)), shininess);
return vec2(diffuse, spec);
}
void main()
{
vec3 lcolor = vec3(0.8,0.2,0.2);
vec2 inten = blinnPhongDir(vec3(1,1,1), 0.5, 0.2, 0.8, 0.8, 80.0);
gl_FragColor = vec4(lcolor * inten.x + vec3(1.0) * inten.y, 1.0);
}
#endif













