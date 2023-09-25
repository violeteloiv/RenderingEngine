#ifndef __VIOLET_VULKAN_RENDERER_API_H_INCLUDED__
#define __VIOLET_VULKAN_RENDERER_API_H_INCLUDED__

#include "../../Renderer/RendererAPI.h"

#include "../../Core/Window.h"

#include "../Vulkan/VulkanDebug.h"
#include "../Vulkan/VulkanInstance.h"
#include "../Vulkan/VulkanSurface.h"

namespace Violet
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual void Init(Ref<Window> p_Window) override;
	private:
		Ref<VulkanInstance> m_Instance;
		Ref<VulkanDebugMessenger> m_DebugMessenger;
		Ref<VulkanSurface> m_Surface;
	};
}

#endif // __VIOLET_VULKAN_RENDERER_API_H_INCLUDED__