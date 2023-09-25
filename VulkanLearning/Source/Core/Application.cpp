#include "../Core/Application.h"

#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Violet
{
	Application::Application()
	{
		m_Window = Window::Create();
	}

	void Application::Run()
	{
		while (!m_Window->ShouldClose())
		{
			// Update Window
			m_Window->OnUpdate();
		}
	}
}