#include "../Vulkan/VulkanRendererAPI.h"

namespace Violet
{
	void VulkanRendererAPI::Init(Ref<Window> p_Window)
	{
		m_Instance = CreateRef<VulkanInstance>();
		m_DebugMessenger = CreateRef<VulkanDebugMessenger>(m_Instance);
		m_Surface = CreateRef<VulkanSurface>(m_Instance, p_Window->GetHandle());
	}
}