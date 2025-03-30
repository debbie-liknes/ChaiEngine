#version 330 core
#define MAX_LIGHTS 16

in vec3 outNorm;
in vec3 FragPos;
out vec4 FragColor;

layout (std140) uniform Light {
    vec3 position;
    vec3 color;
    float intensity;
} lightUBO;

//layout(std140) uniform LightData {
//    int lightCount;
//    Light lights[MAX_LIGHTS];
//} lightUBO;

layout (std140) uniform ColorData {
	vec3 color;
} colorUBO;

void main()
{
   vec3 lightPos = lightUBO.position;
   vec3 lightColor = lightUBO.color;
   float ambientStrength = 0.1;
   vec3 ambient = ambientStrength * lightColor;
   vec3 lightDir = normalize(lightPos - FragPos);
   float diff = max(dot(outNorm, lightDir), 0.0);
   vec3 diffuse = diff * lightColor;
   vec3 result = (ambient + diffuse) * colorUBO.color;
   FragColor = vec4(result, 1.0);
}