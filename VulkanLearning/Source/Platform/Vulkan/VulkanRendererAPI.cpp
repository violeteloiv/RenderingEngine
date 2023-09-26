#include "../Vulkan/VulkanRendererAPI.h"

namespace Violet
{
	void VulkanRendererAPI::Init()
	{
		VulkanAppInfo info = { "Application", 1, 0, 0 };
		m_Instance = CreateScope<VulkanInstance>(info);
	}
}