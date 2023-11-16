#ifndef __VIOLET_VULKAN_RENDERER_API_H_INCLUDED__
#define __VIOLET_VULKAN_RENDERER_API_H_INCLUDED__

#include "../../Renderer/RendererAPI.h"

#include "../../Core/Window.h"

#include "../Vulkan/VulkanInstance.h"
#include "../Vulkan/VulkanLogicalDevice.h"
#include "../Vulkan/VulkanPhysicalDevice.h"

namespace Violet
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual void PreInit(Ref<Window> p_Window) override;
		virtual void Init() override;
	private:
		Ref<Window> m_Window;
		Ref<VulkanInstance> m_Instance;
		Ref<VulkanLogicalDevice> m_Device;
	};
}

#endif // __VIOLET_VULKAN_RENDERER_API_H_INCLUDED__