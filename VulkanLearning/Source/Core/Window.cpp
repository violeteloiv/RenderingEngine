#include "../Core/Window.h"

#include <algorithm>
#include <iostream>
#include <vector>

namespace Core
{
	Window::Window(WindowProperties& p_WindowProps)
		: m_WindowProps(p_WindowProps)
	{
		InitializeWindow();
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_WindowPointer);
	}

	GLFWwindow* Window::GetWindowPointer()
	{
		return m_WindowPointer;
	}

	bool Window::VulkanSupportsGLFWExtensions(uint32_t p_ExtensionCount, const char** p_Extensions)
	{
		// Get all extensions that Vulkan supports.
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		// Convert VkExtensionProperties list into list of c_strs
		std::vector<const char*> extensions_as_cstrs;
		for (const auto& extension : extensions)
			extensions_as_cstrs.push_back(extension.extensionName);

		// Do the check.
		for (uint32_t i = 0; i < p_ExtensionCount; i++)
		{
			for (const auto& extension : extensions)
			{
				if (!strcmp(extension.extensionName, p_Extensions[i]))
					continue;
				else
					break;
			}
		}
		return true;
	}

	void Window::CreateSurface(VkInstance p_Instance)
	{
		if (glfwCreateWindowSurface(p_Instance, m_WindowPointer, nullptr, &m_Surface) != VK_SUCCESS)
			throw std::runtime_error("Failed To Create Window Surface!");
	}

	VkSurfaceKHR Window::GetSurface()
	{
		return m_Surface;
	}

	void Window::InitializeWindow()
	{
		// We need to tell GLFW that we aren't using OPEN_GL. It chooses this by
		// default.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		// TODO: Handle Window Resizing
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		// Create the actual window.
		m_WindowPointer = glfwCreateWindow(m_WindowProps.Width, m_WindowProps.Height, m_WindowProps.Name.c_str(), nullptr, nullptr);
	}
}