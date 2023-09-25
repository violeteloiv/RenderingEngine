#include "../Vulkan/VulkanQueueFamily.h"

#include <vector>

namespace Violet
{
	VulkanQueueFamily VulkanQueueFamily::s_QueueFamily = VulkanQueueFamily();

	bool VulkanQueueFamily::Indices::IsComplete()
	{
		return GraphicsFamily.has_value() && PresentFamily.has_value();
	}

	VulkanQueueFamily::Indices VulkanQueueFamily::FindIndices(VkPhysicalDevice p_PhysicalDevice, VkSurfaceKHR p_Surface)
	{
		VulkanQueueFamily::Indices indices;

		// Assign index to queue familes that could be found.
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(p_PhysicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(p_PhysicalDevice, &queueFamilyCount, queueFamilies.data());

		// Assign indices to queue families.
		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			// Check for Graphics Family
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.GraphicsFamily = i;

			// Check for Present Family
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(p_PhysicalDevice, i, p_Surface, &presentSupport);

			if (presentSupport)
				indices.PresentFamily = i;

			if (indices.IsComplete())
				break;

			i++;
		}

		return indices;
	}
}