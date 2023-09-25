#ifndef __VIOLET_WINDOWS_PLATFORM_WINDOW_H_INCLUDED__
#define __VIOLET_WINDOWS_PLATFORM_WINDOW_H_INCLUDED__

#include "../../Core/Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Violet
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties& p_Properties);
		virtual ~WindowsWindow();
	public:
		uint32_t GetWidth() const override { return m_Data.Width; }
		uint32_t GetHeight() const override { return m_Data.Height; }
		virtual void* GetHandle() const override { return m_WindowHandle; }
	public:
		void OnUpdate() override;
		bool ShouldClose() override;
	private:
		struct WindowData
		{
			std::string Name;
			uint32_t Width, Height;
		};
	private:
		GLFWwindow* m_WindowHandle;
		WindowData m_Data;
	};
}

#endif // __VIOLET_WINDOWS_PLATFORM_WINDOW_H_INCLUDED__