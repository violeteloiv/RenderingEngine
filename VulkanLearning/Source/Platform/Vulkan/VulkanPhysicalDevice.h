#ifndef __VIOLET_VULKAN_PHYSICAL_DEVICE_H_INCLUDED__
#define __VIOLET_VULKAN_PHYSICAL_DEVICE_H_INCLUDED__

#include "../../Core/Pointer.h"

#include "../Vulkan/VulkanInstance.h"
#include "../Vulkan/VulkanSurface.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Violet
{
	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice(Ref<VulkanInstance> p_Instance, Ref<VulkanSurface> p_Surface);
		~VulkanPhysicalDevice();
	public:
		VkPhysicalDevice GetHandle() { return m_PhysicalDevice; }
		int RateDeviceSuitability(VkPhysicalDevice p_PhysicalDevice);
	private:
		VkPhysicalDevice m_PhysicalDevice;
		Ref<VulkanInstance> m_Instance;
		Ref<VulkanSurface> m_Surface;
		std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};
}

#endif // __VIOLET_VULKAN_PHYSICAL_DEVICE_H_INCLUDED__