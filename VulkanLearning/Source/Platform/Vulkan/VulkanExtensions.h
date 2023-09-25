#ifndef __VIOLET_VULKAN_EXTENSIONS_H_INCLUDED__
#define __VIOLET_VULKAN_EXTENSIONS_H_INCLUDED__

#include <set>
#include <stdexcept>
#include <vector>

#include "../../Core/Platform.h"

#include "../Vulkan/VulkanDebug.h"
#include "../Vulkan/VulkanRendererAPI.h"

#ifdef VI_PLATFORM_WINDOWS
	#include "../../Platform/Windows/WindowsExtensions.h"
#endif

namespace Violet
{
	std::vector<const char*> GetRequiredExtensions()
	{
		std::vector<const char*> extensions;

#ifdef VI_PLATFORM_WINDOWS
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = GetGLFWExtensions(glfwExtensionCount);

		if (!SupportsGLFWExtensions(glfwExtensionCount, glfwExtensions))
			throw std::runtime_error("Vulkan Doesn't Support GLFW Extensions!");

		extensions.resize(extensions.size() + glfwExtensionCount);
		for (size_t i = 0; i < glfwExtensionCount; i++)
			extensions.push_back(glfwExtensions[i]);
#endif

		if (ValidationLayersEnabled())
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}

#ifdef VI_PLATFORM_WINDOWS
	bool SupportsGLFWExtensions(uint32_t p_ExtensionCount, const char** p_Extensions)
	{
		// Get all extensions that Vulkan supports.
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		// Convert VkExtensionProperties list into list of c_strs
		std::vector<const char*> extensions_as_cstrs;
		for (const auto& extension : extensions)
			extensions_as_cstrs.push_back(extension.extensionName);

		// Do the check.
		for (uint32_t i = 0; i < p_ExtensionCount; i++)
		{
			for (const auto& extension : extensions)
			{
				if (!strcmp(extension.extensionName, p_Extensions[i]))
					continue;
				else
					break;
			}
		}
		return true;
	}
#endif

	bool CheckDeviceExtensionSupport(VkPhysicalDevice p_Device, std::vector<const char*> p_Extensions)
	{
		// Get the extension count.
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(p_Device, nullptr, &extensionCount, nullptr);

		// Get the extensions.
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(p_Device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(p_Extensions.begin(), p_Extensions.end());

		// Check if the extensions we want exist in our vulkan instance.
		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		return requiredExtensions.empty();
	}
}

#endif // __VIOLET_VULKAN_EXTENSIONS_H_INCLUDED__