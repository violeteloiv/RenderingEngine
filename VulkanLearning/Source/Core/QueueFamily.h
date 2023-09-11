#ifndef __VIOLET_CORE_QUEUE_FAMILY_H_INCLUDED__
#define __VIOLET_CORE_QUEUE_FAMILY_H_INCLUDED__

#include <cstdint>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

namespace Core
{
	namespace Vulkan
	{
		struct QueueFamilyIndices {
			// We use an optional here because we could have some queue families but not require them.
			std::optional<uint32_t> GraphicsFamily;
			std::optional<uint32_t> PresentFamily;

			bool IsComplete();
		};

		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice p_Device, VkSurfaceKHR p_Surface);
	}
	
}

#endif // __VIOLET_CORE_QUEUE_FAMILY_H_INCLUDED__