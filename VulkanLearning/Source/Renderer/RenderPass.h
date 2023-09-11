#ifndef __VIOLET_RENDERER_RENDER_PASS_H_INCLUDED__
#define __VIOLET_RENDERER_RENDER_PASS_H_INCLUDED__

#include <vulkan/vulkan.h>

#include "../Renderer/SwapChain.h"

namespace Renderer
{
	class RenderPass
	{
	public:
		RenderPass() {}
		RenderPass(VkDevice p_Device, VkSurfaceFormatKHR p_Detail);
	public:
		void Shutdown();
		VkRenderPass GetHandle();
	private:
		VkDevice m_Device;
		VkRenderPass m_RenderPass;
	};
}

#endif // __VIOLET_RENDERER_RENDER_PASS_H_INCLUDED__