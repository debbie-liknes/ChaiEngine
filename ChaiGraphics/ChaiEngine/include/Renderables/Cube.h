#pragma once
#include <Renderables/Renderable.h>
#include <glm/glm.hpp>

namespace chai::brew
{
	class CHAIGRAPHICS_EXPORT CubeRO : public Renderable
	{
    public:
        CubeRO(float size = 1.0f) {
            //createMesh(renderer, size);
            //createDefaultMaterial(renderer);
        }

    private:
        void createMesh(float size) {
            //float halfSize = size * 0.5f;

            //// Cube vertices (24 vertices - 4 per face for proper normals)
            //std::vector<Vertex> vertices = {
            //    // Front face (+Z)
            //    {{-halfSize, -halfSize,  halfSize}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            //    {{ halfSize, -halfSize,  halfSize}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            //    {{ halfSize,  halfSize,  halfSize}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            //    {{-halfSize,  halfSize,  halfSize}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

            //    // Back face (-Z)
            //    {{ halfSize, -halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
            //    {{-halfSize, -halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
            //    {{-halfSize,  halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
            //    {{ halfSize,  halfSize, -halfSize}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},

            //    // Left face (-X)
            //    {{-halfSize, -halfSize, -halfSize}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            //    {{-halfSize, -halfSize,  halfSize}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            //    {{-halfSize,  halfSize,  halfSize}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            //    {{-halfSize,  halfSize, -halfSize}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},

            //    // Right face (+X)
            //    {{ halfSize, -halfSize,  halfSize}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            //    {{ halfSize, -halfSize, -halfSize}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            //    {{ halfSize,  halfSize, -halfSize}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            //    {{ halfSize,  halfSize,  halfSize}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},

            //    // Bottom face (-Y)
            //    {{-halfSize, -halfSize, -halfSize}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
            //    {{ halfSize, -halfSize, -halfSize}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
            //    {{ halfSize, -halfSize,  halfSize}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
            //    {{-halfSize, -halfSize,  halfSize}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},

            //    // Top face (+Y)
            //    {{-halfSize,  halfSize,  halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            //    {{ halfSize,  halfSize,  halfSize}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            //    {{ halfSize,  halfSize, -halfSize}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            //    {{-halfSize,  halfSize, -halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
            //};

            //// Cube indices (36 indices - 6 faces * 2 triangles * 3 vertices)
            //std::vector<uint32_t> indices = {
            //    // Front
            //    0, 1, 2,    2, 3, 0,
            //    // Back
            //    4, 5, 6,    6, 7, 4,
            //    // Left
            //    8, 9, 10,   10, 11, 8,
            //    // Right
            //    12, 13, 14, 14, 15, 12,
            //    // Bottom
            //    16, 17, 18, 18, 19, 16,
            //    // Top
            //    20, 21, 22, 22, 23, 20
            //};

            //mesh = renderer->createMesh(vertices, indices);
        }

        void createDefaultMaterial() {
            std::string vertexShader = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aNormal;
            layout (location = 2) in vec2 aTexCoord;
            
            uniform mat4 uModel;
            uniform mat4 uView;
            uniform mat4 uProjection;
            
            out vec3 FragPos;
            out vec3 Normal;
            out vec2 TexCoord;
            
            void main() {
                FragPos = vec3(uModel * vec4(aPos, 1.0));
                Normal = mat3(transpose(inverse(uModel))) * aNormal;
                TexCoord = aTexCoord;
                
                gl_Position = uProjection * uView * vec4(FragPos, 1.0);
            }
        )";

            std::string fragmentShader = R"(
            #version 330 core
            out vec4 FragColor;
            
            in vec3 FragPos;
            in vec3 Normal;
            in vec2 TexCoord;
            
            uniform vec4 uColor;
            uniform sampler2D uTexture;
            uniform bool uHasTexture;
            
            // Simple lighting
            uniform vec3 uLightPos;
            uniform vec3 uLightColor;
            uniform vec3 uViewPos;
            
            void main() {
                vec3 color = uColor.rgb;
                
                if (uHasTexture) {
                    color *= texture(uTexture, TexCoord).rgb;
                }
                
                // Ambient
                float ambientStrength = 0.1;
                vec3 ambient = ambientStrength * uLightColor;
                
                // Diffuse
                vec3 norm = normalize(Normal);
                vec3 lightDir = normalize(uLightPos - FragPos);
                float diff = max(dot(norm, lightDir), 0.0);
                vec3 diffuse = diff * uLightColor;
                
                // Specular
                float specularStrength = 0.5;
                vec3 viewDir = normalize(uViewPos - FragPos);
                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
                vec3 specular = specularStrength * spec * uLightColor;
                
                vec3 result = (ambient + diffuse + specular) * color;
                FragColor = vec4(result, uColor.a);
            }
        )";

            //material = renderer->createMaterial(vertexShader, fragmentShader);

            //// Set default values
            //material->setUniform("uColor", std::array<float, 4>{1.0f, 1.0f, 1.0f, 1.0f});
            //material->setUniform("uHasTexture", 0); // false
            //material->setUniform("uLightPos", std::array<float, 3>{2.0f, 2.0f, 2.0f});
            //material->setUniform("uLightColor", std::array<float, 3>{1.0f, 1.0f, 1.0f});
            //material->setUniform("uViewPos", std::array<float, 3>{0.0f, 0.0f, 3.0f});
        }
	};
}