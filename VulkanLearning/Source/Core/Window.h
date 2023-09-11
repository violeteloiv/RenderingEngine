#ifndef __VIOLET_CORE_WINDOW_H_INCLUDED__
#define __VIOLET_CORE_WINDOW_H_INCLUDED__

#include <cstdlib>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Core
{
	struct WindowProperties
	{
	public:
		const uint32_t Width;
		const uint32_t Height;
		const std::string& Name;
	};

	class Window
	{
	public:
		Window(WindowProperties& p_WindowProps);
		~Window();
	public:
		GLFWwindow* GetWindowPointer();
		bool VulkanSupportsGLFWExtensions(uint32_t p_ExtensionCount, const char** p_Extensions);
		void CreateSurface(VkInstance p_Instance);
		VkSurfaceKHR GetSurface();
	private:
		void InitializeWindow();
	private:
		WindowProperties& m_WindowProps;
		GLFWwindow* m_WindowPointer;
		VkSurfaceKHR m_Surface;
	};
}
 
#endif // __VIOLET_CORE_WINDOW_H_INCLUDED__