#version 420 core

in vec3 v_Dir;
out vec4 FragColor;

layout(binding = 0) uniform samplerCube u_Skybox;

void main()
{
    vec3 dir = normalize(v_Dir);
    vec3 color = texture(u_Skybox, dir).rgb;
    FragColor = vec4(color, 1.0);
}
