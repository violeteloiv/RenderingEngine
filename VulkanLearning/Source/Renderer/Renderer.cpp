#include "../Renderer/Renderer.h"

namespace Renderer
{
	Renderer* Renderer::s_Renderer = nullptr;

	SwapChain Renderer::s_SwapChain;
	RenderPass Renderer::s_RenderPass;
	RenderCommand Renderer::s_RenderCommand;
	GraphicsPipeline Renderer::s_Pipeline;

	Renderer* Renderer::GetInstance()
	{
		if (s_Renderer == nullptr)
			s_Renderer = new Renderer();

		return s_Renderer;
	}

	void Renderer::SetUp(VkDevice p_Device, VkPhysicalDevice p_PhysicalDevice, Core::Window* p_Window)
	{
		// Create the swap chain.
		s_SwapChain = SwapChain(p_Device, p_PhysicalDevice, p_Window);
		// Create the render pass.
		s_RenderPass = RenderPass(p_Device, s_SwapChain.GetSpecifiedDetails().Format);
		// Create the graphics pipeline.
		s_Pipeline = GraphicsPipeline(p_Device, s_SwapChain, s_RenderPass);
		// Generate framebuffers from all these objects.
		s_SwapChain.GenerateFramebuffers(s_RenderPass.GetHandle());

		// Create the command pool and setup render commands.
		s_RenderCommand = RenderCommand(p_Device, p_PhysicalDevice, p_Window);
	}

	void Renderer::Shutdown(VkDevice p_Device)
	{
		// Shutdown Render Commands 
		s_RenderCommand.Shutdown();

		// Destroy Pipeline
		s_Pipeline.Shutdown();
		// Destroy Render Pass
		s_RenderPass.Shutdown();
		// Destroy SwapChain
		s_SwapChain.Shutdown();
	}

	VkCommandBuffer Renderer::RecordCommandBuffer(uint32_t p_ImageIndex)
	{
		return s_RenderCommand.RecordCommandBuffer(p_ImageIndex, s_RenderPass, s_SwapChain, s_Pipeline);
	}

	RenderCommand Renderer::GetRenderCommand()
	{
		return s_RenderCommand;
	}

	SwapChain Renderer::GetSwapChain()
	{
		return s_SwapChain;
	}
}