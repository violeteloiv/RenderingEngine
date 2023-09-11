#ifndef __VIOLET_RENDERER_RENDER_COMMAND_H_INCLUDED__
#define __VIOLET_RENDERER_RENDER_COMMAND_H_INCLUDED__

#include <vulkan/vulkan.h>

#include "../Core/QueueFamily.h"
#include "../Core/Window.h"
#include "../Renderer/GraphicsPipeline.h"
#include "../Renderer/RenderPass.h"
#include "../Renderer/SwapChain.h"

namespace Renderer
{
	class RenderCommand
	{
	public:
		RenderCommand() {};
		RenderCommand(VkDevice p_Device, VkPhysicalDevice p_PhysicalDevice, Core::Window* p_Window);
	public:
		void Shutdown();
	public:
		VkCommandBuffer RecordCommandBuffer(uint32_t p_ImageIndex, RenderPass p_RenderPass, SwapChain p_SwapChain, GraphicsPipeline p_GraphicsPipeline);
		VkCommandBuffer GetHandle();
	private:
		VkCommandPool m_CommandPool;
		VkCommandBuffer m_CommandBuffer;
		VkDevice m_Device;
	};
}

#endif // __VIOLET_RENDERER_RENDER_COMMAND_H_INCLUDED__