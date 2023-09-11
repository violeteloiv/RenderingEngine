#ifndef __VIOLET_CORE_EXTENSIONS_H_INCLUDED__
#define __VIOLET_CORE_EXTENSIONS_H_INCLUDED__

#include <set>
#include <vector>
#include <vulkan/vulkan.h>

namespace Core
{
	namespace Extension
	{
		const std::vector<const char*> DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		bool CheckDeviceExtensionSupport(VkPhysicalDevice p_Device)
		{
			// Get the extension count.
			uint32_t extensionCount;
			vkEnumerateDeviceExtensionProperties(p_Device, nullptr, &extensionCount, nullptr);

			// Get the extensions.
			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(p_Device, nullptr, &extensionCount, availableExtensions.data());

			std::set<std::string> requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

			// Check if the extensions we want exist in our vulkan instance.
			for (const auto& extension : availableExtensions)
				requiredExtensions.erase(extension.extensionName);

			return requiredExtensions.empty();
		}
	}
}

#endif // __VIOLET_CORE_EXTENSIONS_H_INCLUDED__