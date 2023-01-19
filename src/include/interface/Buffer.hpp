#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstdint>

namespace APILearning
{
	class VertexBuffer
	{
	public:
		virtual void Stage() const = 0;
	};

	class IndexBuffer
	{
	public:
		virtual void Stage() const = 0;
		virtual uint32_t GetCount() const = 0;
	protected:
		uint32_t m_Count;
	};
}

#endif // !BUFFER_HPP
