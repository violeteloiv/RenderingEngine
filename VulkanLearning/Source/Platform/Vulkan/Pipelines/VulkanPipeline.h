#ifndef __VIOLET_ENGINE_VULKAN_PIPELINE_H_INCLUDED__
#define __VIOLET_ENGINE_VULKAN_PIPELINE_H_INCLUDED__

#include <vector>

#include "../../Vulkan/VulkanLogicalDevice.h"

#include <vulkan/vulkan.h>

namespace Violet
{
	class VulkanPipeline
	{
	public:
		VulkanPipeline(Ref<VulkanLogicalDevice> p_Device);
		~VulkanPipeline();
	private:
		std::vector<VkDynamicState> m_DynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
	private:
		VkPipelineLayout m_PipelineLayout;
		Ref<VulkanLogicalDevice> m_Device;
	};
}

#endif // __VIOLET_ENGINE_VULKAN_PIPELINE_H_INCLUDED__