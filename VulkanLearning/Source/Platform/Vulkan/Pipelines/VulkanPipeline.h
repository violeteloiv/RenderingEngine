#ifndef __VIOLET_ENGINE_VULKAN_PIPELINE_H_INCLUDED__
#define __VIOLET_ENGINE_VULKAN_PIPELINE_H_INCLUDED__

#include <vector>

#include "../../Vulkan/VulkanLogicalDevice.h"

#include "../../Vulkan/Pipelines/VulkanShader.h"

#include <vulkan/vulkan.h>

namespace Violet
{
	class VulkanPipeline
	{
	public:
		VulkanPipeline(Ref<VulkanLogicalDevice> p_Device, Ref<VulkanShader> p_Shader);
		~VulkanPipeline();
	public:
		void CreateRenderPass();
	private:
		std::vector<VkDynamicState> m_DynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
	private:
		VkPipelineLayout m_PipelineLayout;
		VkRenderPass m_RenderPass;
		VkPipeline m_GraphicsPipeline;
	private:
		Ref<VulkanLogicalDevice> m_Device;
		Ref<VulkanShader> m_Shader;
	};
}

#endif // __VIOLET_ENGINE_VULKAN_PIPELINE_H_INCLUDED__