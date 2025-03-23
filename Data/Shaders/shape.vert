#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

layout (std140) uniform MatrixData {
	mat4 proj;
	mat4 view;
} ubo;

layout (std140) uniform ModelData {
	mat4 ma;
} model;

out vec3 outNorm;
out vec3 FragPos;
void main()
{
   outNorm = aNorm;
   FragPos = vec3(model.ma * vec4(aPos, 1.0));
   gl_Position = ubo.proj * ubo.view * model.ma * vec4(aPos, 1.0);
}