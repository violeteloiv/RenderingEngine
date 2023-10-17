#include "../Vulkan/VulkanRendererAPI.h"

namespace Violet
{
	void VulkanRendererAPI::Init()
	{
		// Setup the Vulkan Instance.
		VulkanAppInfo info = { "Application", 1, 0, 0 };
		m_Instance = CreateRef<VulkanInstance>(info);
#ifndef NDEBUG
		m_Instance->CreateDebugMessenger();
#endif
		// Create the device for vulkan to use.
		m_Device = CreateScope<VulkanLogicalDevice>(m_Instance);
	}
}