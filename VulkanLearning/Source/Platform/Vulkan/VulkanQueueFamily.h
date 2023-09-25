#ifndef __VIOLET_VULKAN_QUEUE_FAMILY_H_INCLUDED__
#define __VIOLET_VULKAN_QUEUE_FAMILY_H_INCLUDED__

#include <optional>
#include <vulkan/vulkan.h>

namespace Violet
{
	class VulkanQueueFamily
	{
	public:
		struct Indices {
		public:
			// We use an optional here because we could have some queue families but not require them.
			std::optional<uint32_t> GraphicsFamily;
			std::optional<uint32_t> PresentFamily;
		public:
			bool IsComplete();
		};
	public:
		VulkanQueueFamily() = default;
	public:
		static Indices FindIndices(VkPhysicalDevice p_PhysicalDevice, VkSurfaceKHR p_Surface);
	private:
		static VulkanQueueFamily s_QueueFamily;
	};
}

#endif // __VIOLET_VULKAN_QUEUE_FAMILY_H_INCLUDED__