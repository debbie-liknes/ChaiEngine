#version 450
layout(location = 0) in vec3 vViewDir;
layout(location = 0) out vec4 outColor;

layout(set = 3, binding = 0) uniform samplerCube envMap;

vec3 acesFilm(vec3 x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

    return clamp(
        (x * (a * x + b)) /
        (x * (c * x + d) + e),
        0.0,
        1.0);
}

void main()
{
    vec3 dir = normalize(vViewDir);

    vec3 color = texture(envMap, dir).rgb;

    color = acesFilm(color);
    color = pow(color, vec3(1.0 / 2.2));

    outColor = vec4(color, 1.0);
}