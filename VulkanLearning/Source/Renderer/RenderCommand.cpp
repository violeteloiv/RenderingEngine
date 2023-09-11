#include "../Renderer/RenderCommand.h"

#include <stdexcept>

namespace Renderer
{
	RenderCommand::RenderCommand(VkDevice p_Device, VkPhysicalDevice p_PhysicalDevice, Core::Window* p_Window)
		: m_Device(p_Device)
	{
		Core::Vulkan::QueueFamilyIndices queueFamilyIndices = Core::Vulkan::FindQueueFamilies(p_PhysicalDevice, p_Window->GetSurface());

		// Create the command pool info.
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

		// Create the actual command pool.
		if (vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
			throw std::runtime_error("Failed To Create Command Pool!");

		// Create the command buffer info.
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		// Create the command buffer.
		if (vkAllocateCommandBuffers(m_Device, &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed To Allocate Command Buffer!");
	}

	void RenderCommand::Shutdown()
	{
		// Destroy Command Pool
		vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
	}

	VkCommandBuffer RenderCommand::RecordCommandBuffer(uint32_t p_ImageIndex, RenderPass p_RenderPass, SwapChain p_SwapChain, GraphicsPipeline p_GraphicsPipeline)
	{
		// Create begin info.
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		// Begin the command buffer.
		if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed To Begin Recording Command Buffer!");

		// Start the render pass!
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = p_RenderPass.GetHandle();
		renderPassInfo.framebuffer = p_SwapChain.GetFramebuffers()[p_ImageIndex];
		// Define the render area.
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = p_SwapChain.GetSpecifiedDetails().Extent;

		// TODO: Make it so that the renderer controls this value.
		VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		// Begin the render pass :heart_eyes:
		vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Bind the pipeline
		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, p_GraphicsPipeline.GetHandle());

		// Define the viewport
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(p_SwapChain.GetSpecifiedDetails().Extent.width);
		viewport.height = static_cast<float>(p_SwapChain.GetSpecifiedDetails().Extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);

		// Define the scissor
		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = p_SwapChain.GetSpecifiedDetails().Extent;
		vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);

		// DRAW THE TRIANGLE
		vkCmdDraw(m_CommandBuffer, 3, 1, 0, 0);

		// End Render Pass
		vkCmdEndRenderPass(m_CommandBuffer);
		if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed To Record Command Buffer!");

		return m_CommandBuffer;
	}

	VkCommandBuffer RenderCommand::GetHandle()
	{
		return m_CommandBuffer;
	}
}