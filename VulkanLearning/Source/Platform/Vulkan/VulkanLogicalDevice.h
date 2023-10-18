#ifndef __VIOLET_PLATFORM_VULKAN_LOGICAL_DEVICE_H_INCLUDED__
#define __VIOLET_PLATFORM_VULKAN_LOGICAL_DEVICE_H_INCLUDED__

#include <vulkan/vulkan.h>

#include "../Vulkan/VulkanInstance.h"
#include "../Vulkan/VulkanPhysicalDevice.h"

namespace Violet
{
	class VulkanLogicalDevice
	{
	public:
		VulkanLogicalDevice(Ref<VulkanInstance> p_Instance);
		~VulkanLogicalDevice();
	private:
		VkDevice m_DeviceHandle;
		VkQueue m_GraphicsQueueHandle;
		VkQueue m_PresentQueueHandle;
	private:
		Ref<VulkanInstance> m_Instance;
		Scope<VulkanPhysicalDevice> m_PhysicalDevice;
	};
}

#endif // __VIOLET_PLATFORM_VULKAN_LOGICAL_DEVICE_H_INCLUDED__