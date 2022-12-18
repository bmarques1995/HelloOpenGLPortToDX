#ifndef GRAPHICS_CONTEXT_HPP
#define GRAPHICS_CONTEXT_HPP

namespace APILearning
{
	class GraphicsContext
	{
	public:
		virtual void Update() = 0;
		virtual void SetClearColor(float r, float g, float b, float a) = 0;
	};
}

#endif // GRAPHICS_CONTEXT_HPP
