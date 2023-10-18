#include "../Vulkan/VulkanRendererAPI.h"

namespace Violet
{
	void VulkanRendererAPI::PreInit(Ref<Window> p_Window)
	{
		m_Window = p_Window;
	}

	void VulkanRendererAPI::Init()
	{
		// Setup the Vulkan Instance.
		VulkanAppInfo info = { "Application", 1, 0, 0 };
		m_Instance = CreateRef<VulkanInstance>(info);
#ifndef NDEBUG
		m_Instance->CreateDebugMessenger();
#endif

#ifdef VI_PLATFORM_WINDOWS
		// Define the surface for the program to display to.
		m_Instance->InstantiateSurface(m_Window);
#endif

		// Create the device for vulkan to use.
		m_Device = CreateScope<VulkanLogicalDevice>(m_Instance);
	}
}