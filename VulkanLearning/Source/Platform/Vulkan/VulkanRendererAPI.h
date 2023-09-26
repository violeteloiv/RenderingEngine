#ifndef __VIOLET_VULKAN_RENDERER_API_H_INCLUDED__
#define __VIOLET_VULKAN_RENDERER_API_H_INCLUDED__

#include "../../Renderer/RendererAPI.h"

#include "../../Core/Window.h"

#include "../Vulkan/VulkanInstance.h"

namespace Violet
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
	private:
		Scope<VulkanInstance> m_Instance;
	};
}

#endif // __VIOLET_VULKAN_RENDERER_API_H_INCLUDED__