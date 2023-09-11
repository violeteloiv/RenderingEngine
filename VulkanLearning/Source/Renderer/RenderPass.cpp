#include "../Renderer/RenderPass.h"

#include <stdexcept>

namespace Renderer
{
	RenderPass::RenderPass(VkDevice p_Device, VkSurfaceFormatKHR p_Detail)
		: m_Device(p_Device)
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = p_Detail.format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		// Determines what to do with the data before and after rendering. Here we
		// clear the values to a constant at the start, and then store rendered
		// contents in memory.
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		// We aren't doing anything with stencil buffers.
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		// This becomes more important in texturing.
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// A Render Pass can consist of subpasses. For example, post-processing
		// affects. We are going to stick to a singular subpass. Subpasses reference
		// other attachments.
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		// Render pass creation.
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkRenderPass pass;
		if (vkCreateRenderPass(p_Device, &renderPassInfo, nullptr, &pass) != VK_SUCCESS)
			throw std::runtime_error("Failed To Create Render Pass!");

		m_RenderPass = pass;
	}

	void RenderPass::Shutdown()
	{
		vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
	}

	VkRenderPass RenderPass::GetHandle()
	{
		return m_RenderPass;
	}
}