// camera_ubo.glsl
#pragma once

struct CameraData {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec3 position;
};