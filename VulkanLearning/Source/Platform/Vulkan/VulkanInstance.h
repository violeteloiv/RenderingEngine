#ifndef __VIOLET_VULKAN_INSTANCE_H_INCLUDED__
#define __VIOLET_VULKAN_INSTANCE_H_INCLUDED__

#include <vulkan/vulkan.h>

namespace Violet
{
	class VulkanInstance
	{
	public:
		VulkanInstance();
		~VulkanInstance();
	public:
		VkInstance GetHandle() { return m_InstanceHandle; }
	private:
		VkInstance m_InstanceHandle;
	};
}

#endif // __VIOLET_VULKAN_INSTANCE_H_INCLUDED__