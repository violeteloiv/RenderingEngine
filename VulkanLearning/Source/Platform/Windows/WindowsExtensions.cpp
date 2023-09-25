#include "../Windows/WindowsExtensions.h"

#include <GLFW/glfw3.h>

namespace Violet
{
	const char** GetGLFWExtensions(uint32_t p_ExtensionCount)
	{
		return glfwGetRequiredInstanceExtensions(&p_ExtensionCount);
	}
}