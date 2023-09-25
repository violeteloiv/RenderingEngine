#ifndef __VIOLET_VULKAN_RENDERER_API_H_INCLUDED__
#define __VIOLET_VULKAN_RENDERER_API_H_INCLUDED__

#include "../../Renderer/RendererAPI.h"

#include "../../Core/Window.h"

namespace Violet
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual void Init(Ref<Window> p_Window) override;
	};
}

#endif // __VIOLET_VULKAN_RENDERER_API_H_INCLUDED__