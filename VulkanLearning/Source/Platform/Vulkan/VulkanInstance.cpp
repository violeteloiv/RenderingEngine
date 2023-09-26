#include "../Vulkan/VulkanInstance.h"

#include <stdexcept>

namespace Violet
{
	VulkanInstance::VulkanInstance(VulkanAppInfo& p_AppInfo)
	{
		// Supply some application information.
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = p_AppInfo.AppName.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(
			p_AppInfo.VersionMajor,
			p_AppInfo.VersionMinor,
			p_AppInfo.VersionRevision
		);
		appInfo.pEngineName = "Violet Engine";
		appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// Supply the instance creation information.
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// Platform Specific Extensions
#ifdef VI_PLATFORM_WINDOWS
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = GetGLFWExtensions(&glfwExtensionCount);
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
#else
		createInfo.enabledExtensionCount = 0;
		createInfo.ppEnabledExtensionNames = nullptr;
#endif

		createInfo.ppEnabledLayerNames = nullptr;
		createInfo.enabledLayerCount = 0;

		// Create the instance.
		if (vkCreateInstance(&createInfo, nullptr, &m_InstanceHandle) != VK_SUCCESS)
			throw std::runtime_error("[ERROR] VulkanInstance - Failed To Create Instance.");
	}

	VulkanInstance::~VulkanInstance()
	{

	}
}