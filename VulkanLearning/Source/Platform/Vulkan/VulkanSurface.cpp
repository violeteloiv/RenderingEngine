#include "../Vulkan/VulkanSurface.h"

#include "../../Core/Platform.h"

#include <stdexcept>

#ifdef VI_PLATFORM_WINDOWS
	#include <GLFW/glfw3.h>
#endif

namespace Violet
{
	VulkanSurface::VulkanSurface(Ref<VulkanInstance> p_Instance, void* p_WindowHandle)
		: m_Instance(p_Instance)
	{
#ifdef VI_PLATFORM_WINDOWS
		if (glfwCreateWindowSurface(p_Instance->GetHandle(), (GLFWwindow*)p_WindowHandle, nullptr, &m_SurfaceHandle) != VK_SUCCESS)
			throw std::runtime_error("Failed To Create Window Surface!");
#endif
	}

	VulkanSurface::~VulkanSurface()
	{
		vkDestroySurfaceKHR(m_Instance->GetHandle(), m_SurfaceHandle, nullptr);
	}
}