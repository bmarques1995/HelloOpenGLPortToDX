#ifndef SHADER_HPP
#define SHADER_HPP

#include <vector>
#include <cassert>
#include <cstdint>
#include <string>

namespace APILearning
{
	enum class ShaderDataType
	{
		None = 0,
		Float,
		Float2,
		Float3,
		Float4,
		Uint,
		Uint2,
		Uint3,
		Uint4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float: return 4;
		case ShaderDataType::Float2: return 4 * 2;
		case ShaderDataType::Float3: return 4 * 3;
		case ShaderDataType::Float4: return 4 * 4;
		case ShaderDataType::Uint: return 4;
		case ShaderDataType::Uint2: return 4 * 2;
		case ShaderDataType::Uint3: return 4 * 3;
		case ShaderDataType::Uint4: return 4 * 4;
		case ShaderDataType::Bool: return 1;
		default: assert(false); return 0;
		}
	}

	struct BufferElement
	{
		std::string Name;
		uint32_t Offset;
		uint32_t Size;
		ShaderDataType Type;
		bool Normalized;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{}

		uint32_t GetElementCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Bool:
			case ShaderDataType::Float:
			case ShaderDataType::Uint:
				return 1Ui32;
			case ShaderDataType::Float2:
			case ShaderDataType::Uint2:
				return 2Ui32;
			case ShaderDataType::Float3:
			case ShaderDataType::Uint3:
				return 3Ui32;
			case ShaderDataType::Float4:
			case ShaderDataType::Uint4:
				return 4Ui32;
			default:
				return 0Ui32;
			}
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferElement>& elements) 
			: m_Elements(elements)
		{
			CalculateOffsetAndStride();
		}

		inline const uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

	private:

		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class Shader
	{
	public:
		virtual void Stage() const = 0;
		virtual uint32_t GetStride() const = 0;
		virtual uint32_t GetOffset() const = 0;
	};
}

#endif // !SHADER_HPP
