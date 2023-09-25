#ifndef __VIOLET_RENDERER_H_INCLUDED__
#define __VIOLET_RENDERER_H_INCLUDED__

#include <vulkan/vulkan.h>

#include "../Core/Window.h"
#include "../Renderer/GraphicsPipeline.h"
#include "../Renderer/RenderCommand.h"
#include "../Renderer/RenderPass.h"
#include "../Renderer/SwapChain.h"

namespace Renderer
{
	class Renderer
	{
	protected:
		Renderer() {}
	protected:
		static SwapChain s_SwapChain;
		static RenderPass s_RenderPass;
		static GraphicsPipeline s_Pipeline;
		static RenderCommand s_RenderCommand;
		static Renderer* s_Renderer;
	public:
		Renderer(Renderer& p_Other) = delete;
		void operator=(const Renderer&) = delete;
	public:
		static void SetUp(VkDevice p_Device, VkPhysicalDevice p_PhysicalDevice, Violet::Ref<Violet::Window> p_Window);
		static void Shutdown(VkDevice p_Device);
		static VkCommandBuffer RecordCommandBuffer(uint32_t p_ImageIndex);
		static RenderCommand GetRenderCommand();
		static SwapChain GetSwapChain();
	public:
		static Renderer* GetInstance();
	};
}

#endif // __VIOLET_RENDERER_H_INCLUDED__