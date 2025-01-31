#pragma once
#include <ChaiApiExport.h>
#include <Core/Containers.h>
#include <Core/MemoryTypes.h>
#include <glm/glm.hpp>
#include <concepts>

template <typename T>
concept AllowedType = std::same_as<T, int> ||
std::same_as<T, unsigned int> ||
std::same_as<T, float> ||
std::same_as<T, glm::vec2> ||
std::same_as<T, glm::vec3>;

namespace CGraphics
{
	class CHAIAPI_EXPORT VertexBufferBase {
	public:
		virtual ~VertexBufferBase() = default;

		// Returns the size of a single element in bytes (needed for OpenGL)
		virtual size_t getElementSize() const = 0;

		// Returns the number of elements stored
		virtual size_t getElementCount() const = 0;

		// Returns a raw pointer to the data
		virtual const void* getRawData() const = 0;
	};

	// Templated derived class for storing vertex buffer data
	template <typename T>
	class VertexBuffer : public VertexBufferBase {
	public:
		Core::CVector<T> data;

		VertexBuffer() {}
		virtual ~VertexBuffer() = default;
		VertexBuffer(Core::CVector<T> d) : data(d) {}

		size_t getElementSize() const override {
			return sizeof(T); // Size of one element
		}

		size_t getElementCount() const override {
			return data.size(); // Number of elements
		}

		const void* getRawData() const override {
			return data.data(); // Pointer to raw data
		}
	};

	template <typename T>
	using VBO = VertexBuffer<T>;

	template <typename T>
	using SharedVBO = Core::CSharedPtr<VBO<T>>;

	template <typename T>
	SharedVBO<T> createVertexBuffer()
	{
		return std::make_shared<VertexBuffer<T>>();
	}

	template <typename T>
	SharedVBO<T> createVertexBuffer(Core::CVector<T> data)
	{
		return std::make_shared<VertexBuffer<T>>(data);
	}
}