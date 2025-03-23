#pragma once
#include <ChaiGraphicsExport.h>
#include <string>
#include <Core/TypeHelpers.h>
#include <Core/MemoryTypes.h>

namespace chai::brew
{
	class CHAIGRAPHICS_EXPORT UniformBufferBase {
	public:
		std::string name;
		virtual ~UniformBufferBase();

		// Returns the size of a single element in bytes (needed for OpenGL)
		virtual size_t getElementSize() const = 0;

		// Returns a raw pointer to the data
		virtual const void* getRawData() const = 0;
	};

	// Templated derived class for storing uniform buffer data
	template <typename T>
	class UniformBuffer : public UniformBufferBase {
	public:
		T data;

		UniformBuffer(PrimDataType underlyingType) :
			m_underlyingType(underlyingType)
		{}
		virtual ~UniformBuffer() = default;

		UniformBuffer(T d, PrimDataType underlyingType) :
			data(d), m_underlyingType(underlyingType)
		{}

		size_t getElementSize() const override {
			return sizeof(T); // Size of one element
		}

		const void* getRawData() const override {
			return &data; // Pointer to raw data
		}

	private:
		PrimDataType m_underlyingType = DataType::FLOAT;
	};

	template <typename T>
	using UBO = UniformBuffer<T>;

	template <typename T>
	using SharedUBO = chai::CSharedPtr<UBO<T>>;

	template <typename T>
	SharedUBO<T> createUniformBuffer(PrimDataType underlyingType)
	{
		return std::make_shared<UniformBuffer<T>>(underlyingType);
	}
}