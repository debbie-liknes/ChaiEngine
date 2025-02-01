#pragma once
#include <ChaiApiExport.h>
#include <Core/Containers.h>
#include <Core/MemoryTypes.h>
#include <glm/glm.hpp>
#include <Core/TypeHelpers.h>

namespace CGraphics
{
	enum DataType
	{
		FLOAT,
		INT,
		UNSIGNED_INT
	};

	class CHAIAPI_EXPORT VertexBufferBase {
	public:
		virtual ~VertexBufferBase() = default;

		// Returns the size of a single element in bytes (needed for OpenGL)
		virtual size_t getElementSize() const = 0;

		// Returns the number of elements stored
		virtual size_t getElementCount() const = 0;

		// Returns a raw pointer to the data
		virtual const void* getRawData() const = 0;

		virtual std::size_t getNumElementsInType() const = 0;

		virtual DataType getUnderlyingType() const = 0;
	};

	// Templated derived class for storing vertex buffer data
	template <typename T>
	class VertexBuffer : public VertexBufferBase {
	public:
		Core::CVector<T> data;

		VertexBuffer(DataType underlyingType, size_t numElements) :
			m_underlyingType(underlyingType), m_numElements(numElements)
		{}
		virtual ~VertexBuffer() = default;
		VertexBuffer(Core::CVector<T> d, DataType underlyingType, size_t numElements) : 
			data(d), m_underlyingType(underlyingType), m_numElements(numElements)
		{}

		size_t getElementSize() const override {
			return sizeof(T); // Size of one element
		}

		size_t getElementCount() const override {
			return data.size(); // Number of elements
		}

		const void* getRawData() const override {
			return data.data(); // Pointer to raw data
		}

		std::size_t getNumElementsInType() const override {
			return m_numElements;
		}

		DataType getUnderlyingType() const override {
			return m_underlyingType;
		}

	private:
		size_t m_numElements = 1;
		DataType m_underlyingType = DataType::FLOAT;
	};

	template <typename T>
	using VBO = VertexBuffer<T>;

	template <typename T>
	using SharedVBO = Core::CSharedPtr<VBO<T>>;

	template <typename T>
	SharedVBO<T> createVertexBuffer(DataType underlyingType, size_t numElements)
	{
		return std::make_shared<VertexBuffer<T>>(underlyingType, numElements);
	}

	//i can simplify these params with template meta programming
	//later ;)
	template <typename T>
	SharedVBO<T> createVertexBuffer(Core::CVector<T> data, DataType underlyingType, size_t numElements)
	{
		return std::make_shared<VertexBuffer<T>>(data, underlyingType, numElements);
	}
}