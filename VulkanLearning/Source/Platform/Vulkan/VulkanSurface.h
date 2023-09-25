#ifndef __VIOLET_VULKAN_SURFACE_H_INCLUDED__
#define __VIOLET_VULKAN_SURFACE_H_INCLUDED__

#include <vulkan/vulkan.h>

#include "../../Core/Window.h"

#include "../Vulkan/VulkanInstance.h"

namespace Violet
{
	class VulkanSurface
	{
	public:
		VulkanSurface(Ref<VulkanInstance> p_Instance, void* p_WindowHandle);
		~VulkanSurface();
	public:
		VkSurfaceKHR GetHandle() { return m_SurfaceHandle; }
	private:
		VkSurfaceKHR m_SurfaceHandle;
		Ref<VulkanInstance> m_Instance;
	};
}

#endif // __VIOLET_VULKAN_SURFACE_H_INCLUDED__