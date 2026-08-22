// lighting.glsl
#pragma once

struct LightData {
    vec4 direction; // .xyz = direction
    vec4 color;     // .rgb = color of light, w = stength
    mat4 view;
    mat4 lightSpaceProj;
};