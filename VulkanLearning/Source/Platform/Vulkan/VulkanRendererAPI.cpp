#include "../Vulkan/VulkanRendererAPI.h"

namespace Violet
{
	void VulkanRendererAPI::Init()
	{
		// Setup the Vulkan Instance.
		VulkanAppInfo info = { "Application", 1, 0, 0 };
		m_Instance = CreateScope<VulkanInstance>(info);
#ifndef NDEBUG
		m_Instance->CreateDebugMessenger();
#endif
	}
}