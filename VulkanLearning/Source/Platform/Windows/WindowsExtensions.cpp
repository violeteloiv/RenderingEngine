#include "../Windows/WindowsExtensions.h"

#include <GLFW/glfw3.h>

#include <vector>

namespace Violet
{
	const char** GetGLFWExtensions(uint32_t* p_Count)
	{
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(p_Count);
		return glfwExtensions;
	}
}