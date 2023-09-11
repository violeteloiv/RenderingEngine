#ifndef __VIOLET_RENDERER_GRAPHICS_PIPELINE_H_INCLUDED__
#define __VIOLET_RENDERER_GRAPHICS_PIPELINE_H_INCLUDED__

#include "../Renderer/RenderPass.h"
#include "../Renderer/SwapChain.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Renderer
{
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline() {}
		GraphicsPipeline(VkDevice p_Device, SwapChain p_SwapChain, RenderPass p_RenderPass);
	public:
		void Shutdown();
		VkPipeline GetHandle();
	private:
		void FindShaderPipelineStages();
		VkShaderModule CreateShaderModule(const std::vector<char>& p_Code);
	private:
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;
	private:
		VkDevice m_Device;
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStageCreateInfos;
	private:
		VkShaderModule m_VertShaderCode;
		VkShaderModule m_FragShaderCode;
	};
}

#endif // __VIOLET_RENDERER_GRAPHICS_PIPELINE_H_INCLUDED__