#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>
#include <memory>
#include <glm/glm.hpp>
#include <ChaiEngine/UniformBuffer.h>

namespace chai::brew
{
	class CHAIGRAPHICS_EXPORT Renderable
	{
    public:
        Renderable() {
			transform = glm::mat4(1.0f); // Identity matrix
        }

        virtual ~Renderable() = default;

        // Getters
        std::shared_ptr<IMesh> getMesh() const { return mesh; }
        std::shared_ptr<IMaterial> getMaterial() const { return material; }
        const glm::mat4& getTransform() const { return transform; }

        // Transform methods
        void setPosition(float x, float y, float z) {
			transform[3] = glm::vec4(x, y, z, 1.0f);
        }

        void setScale(float x, float y, float z) {
			transform[0][0] = x;
        }

        void setUniformScale(float scale) {
            setScale(scale, scale, scale);
        }

        // Material property shortcuts
        void setColor(float r, float g, float b, float a = 1.0f) {
            if (material) {
                //material->setUniform("uColor", createUniformBuffer<glm::vec4>(PrimDataType::FLOAT));
            }
        }

        void setTexture(const std::string& samplerName, uint32_t textureId) {
            if (material) {
                material->setTexture(samplerName, textureId);
            }
        }
	protected:
		std::shared_ptr<IMesh> mesh;
		std::shared_ptr<IMaterial> material;
		glm::mat4 transform;
	};
}