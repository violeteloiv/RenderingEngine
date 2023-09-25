#include "../Windows/WindowsWindow.h"

#include "../../Renderer/RendererAPI.h"

namespace Violet
{
	WindowsWindow::WindowsWindow(const WindowProperties& p_Properties)
	{
		// Initialize the window, first call for any program with a GLFW window!
		glfwInit();

		m_Data.Width = p_Properties.Width;
		m_Data.Height = p_Properties.Height;
		m_Data.Name = p_Properties.Name;

		// Tell GLFW whether we are using OpenGL.
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::OpenGL:
				break;
			default:
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}
		// TODO: Handle Window Resizing
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		// Create the actual window.
		m_WindowHandle = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Name.c_str(), nullptr, nullptr);
	}

	WindowsWindow::~WindowsWindow()
	{
		// Destroy Window
		glfwDestroyWindow(m_WindowHandle);

		// Terminate GLFW
		glfwTerminate();
	}

	void WindowsWindow::OnUpdate()
	{
		// Poll for events on GLFW.
		glfwPollEvents();
	}

	bool WindowsWindow::ShouldClose()
	{
		return glfwWindowShouldClose(m_WindowHandle);
	}
}