#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <cstdint>
#include <string>
#include <any>

namespace APILearning
{
	struct WindowProps
	{
		uint32_t Width;
		uint32_t Height;
		std::string Title;
		WindowProps(std::string title = "Learn D3D", uint32_t width = 1280, uint32_t height = 720) :
			Title(title), Width(width), Height(height)
		{}
	};

	class Window
	{
	public:
		virtual std::any GetNativeWindow() = 0;
		virtual void Update() = 0;
		virtual uint32_t GetWidth() = 0;
		virtual uint32_t GetHeight() = 0;
		virtual bool ShouldClose() = 0;
	};
}

#endif // WINDOW_HPP
