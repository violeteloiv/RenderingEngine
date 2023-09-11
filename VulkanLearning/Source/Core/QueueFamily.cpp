#include "../Core/QueueFamily.h"

namespace Core
{
	namespace Vulkan
	{
		bool QueueFamilyIndices::IsComplete()
		{
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}

		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice p_Device, VkSurfaceKHR p_Surface)
		{
			QueueFamilyIndices indices;

			// Assign index to queue familes that could be found.
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(p_Device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(p_Device, &queueFamilyCount, queueFamilies.data());

			// Assign indices to queue families.
			int i = 0;
			for (const auto& queueFamily : queueFamilies)
			{
				// Check for Graphics Family
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					indices.GraphicsFamily = i;

				// Check for Present Family
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(p_Device, i, p_Surface, &presentSupport);

				if (presentSupport)
					indices.PresentFamily = i;

				if (indices.IsComplete())
					break;

				i++;
			}

			return indices;
		}
	}
}