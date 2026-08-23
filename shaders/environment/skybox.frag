#version 450

#include <color/tonemap.glsl>

layout(location = 0) in vec3 vViewDir;
layout(location = 0) out vec4 outColor;

layout(set = 3, binding = 0) uniform samplerCube envMap;

void main()
{
    vec3 dir = normalize(vViewDir);

    vec3 color = texture(envMap, dir).rgb;

    color = acesFilm(color);
    color = pow(color, vec3(1.0 / 2.2));

    outColor = vec4(color, 1.0);
}