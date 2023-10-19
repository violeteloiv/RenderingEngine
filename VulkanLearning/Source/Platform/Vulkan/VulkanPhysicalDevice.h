#ifndef __VIOLET_PLATFORM_VULKAN_PHYSICAL_DEVICE_H_INCLUDED__
#define __VIOLET_PLATFORM_VULKAN_PHYSICAL_DEVICE_H_INCLUDED__

#include <vulkan/vulkan.h>

#include "../../Core/Pointer.h"

#include "../Vulkan/VulkanInstance.h"
#include "../Vulkan/VulkanSwapChain.h"

#include <optional>

namespace Violet
{
	struct QueueFamilyIndices
	{
	public:
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;
	public:
		bool IsComplete() {
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}
	};

	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice(Ref<VulkanInstance> p_Instance);
		~VulkanPhysicalDevice() = default;
	private:
		uint32_t RatePhysicalDevice(VkPhysicalDevice p_PhysicalDevice);
	public:
		static QueueFamilyIndices FindQueueFamilies(Ref<VulkanInstance> p_Instance, VkPhysicalDevice p_PhysicalDevice);
	public:
		VkPhysicalDevice GetHandle() { return m_PhysicalDeviceHandle; }
	private:
		VkPhysicalDevice m_PhysicalDeviceHandle = VK_NULL_HANDLE;
		QueueFamilyIndices m_Indices;
		Ref<VulkanInstance> m_Instance;
	};
}

#endif // __VIOLET_PLATFORM_VULKAN_PHYSICAL_DEVICE_H_INCLUDED__