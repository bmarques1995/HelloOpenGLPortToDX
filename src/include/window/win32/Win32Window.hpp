#ifndef WIN32_WINDOW_HPP
#define WIN32_WINDOW_HPP

#include "interface/Window.hpp"
#include <windows.h>

namespace APILearning
{
	class Win32Window : public Window
	{
	public:
		Win32Window(WindowProps props = WindowProps());
		~Win32Window();

		virtual void Update() override;
		virtual uint32_t GetWidth() override;
		virtual uint32_t GetHeight() override;
		virtual bool ShouldClose() override;
		virtual std::any GetNativeWindow() override;
	private:
		void CreateWindowClass(HINSTANCE* instance);
		static void AdjustDimensions(LPRECT originalDimensions, DWORD flags);

		HWND m_WindowHandle;
		WNDCLASSEXW m_WindowClass = {0};
		MSG m_MSG = { 0 };

		bool m_ShouldClose;
		uint32_t m_Width;
		uint32_t m_Height;
		std::wstring m_Title;
	};
}

#endif // WINDOW_HPP
