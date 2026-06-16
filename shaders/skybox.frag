#version 450
layout(location = 0) in vec3 vViewDir;
layout(location = 0) out vec4 outColor;

layout(set = 3, binding = 0) uniform samplerCube envMap;

void main() {
    vec3 dir = normalize(vViewDir);
    vec3 color = texture(envMap, dir).rgb;

    // TODO: tonemap + sRGB encode to match your main pass exactly.
    // Use the SAME ACES + pow(1/2.2) you put in the PBR shader, or the skybox
    // won't match the lit scene. (If your swapchain is _SRGB, drop the pow.)
    // color = acesFilm(color * uExposure);
    // color = pow(color, vec3(1.0/2.2));

    outColor = vec4(color, 1.0);
}