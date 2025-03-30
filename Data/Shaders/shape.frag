#version 330 core
#define MAX_LIGHTS 16

in vec3 outNorm;
in vec3 FragPos;
out vec4 FragColor;

struct Light {
    vec3 position;
    float intensity;
    vec3 color;
    float radius;
};

layout(std140) uniform LightData {
    int lightCount;
    Light lights[MAX_LIGHTS];
} lightUBO;

layout (std140) uniform ColorData {
	vec3 color;
} colorUBO;

void main()
{
   vec3 lightPos = vec3(10.f, 10.f, 10.f);
   vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
   vec3 objColor = vec3(1.0f, 0.5f, 0.31f);
   vec3 viewPosition = vec3(0.0, 0.0, 5.0);
   float ambientStrength = 0.1;
   vec3 ambient = ambientStrength * lightColor;
   vec3 lightDir = normalize(lightPos - FragPos);
   float diff = max(dot(outNorm, lightDir), 0.0);
   vec3 diffuse = diff * lightColor;
   vec3 result = (ambient + diffuse) * objColor;
   FragColor = vec4(result, 1.0);
}