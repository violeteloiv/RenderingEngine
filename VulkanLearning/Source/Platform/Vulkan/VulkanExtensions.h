#ifndef __VIOLET_PLATFORM_VULKAN_EXTENSIONS_H_INCLUDED__
#define __VIOLET_PLATFORM_VULKAN_EXTENSIONS_H_INCLUDED__

#include "../../Core/Platform.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Violet
{
	std::vector<const char*> GetRequiredExtensions()
	{
		std::vector<const char*> extensions;

		// Platform specific extensions.
#ifdef VI_PLATFORM_WINDOWS
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = GetGLFWExtensions(&glfwExtensionCount);
		for (uint32_t i = 0; i < glfwExtensionCount; i++)
			extensions.push_back(glfwExtensions[i]);
#endif

		// Debug specific extensions.
#ifndef NDEBUG
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		return extensions;
	}

	namespace DebugUtilsEXT
	{
		VkResult CreateDebugUtilsMessenger(
			VkInstance p_Instance,
			const VkDebugUtilsMessengerCreateInfoEXT* p_CreateInfo,
			const VkAllocationCallbacks* p_Allocator,
			VkDebugUtilsMessengerEXT* p_DebugMessenger)
		{
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_Instance, "vkCreateDebugUtilsMessengerEXT");
			if (func != nullptr)
				return func(p_Instance, p_CreateInfo, p_Allocator, p_DebugMessenger);
			else
				return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		void DestroyDebugUtilsMessenger(
			VkInstance p_Instance,
			VkDebugUtilsMessengerEXT p_DebugMessenger,
			const VkAllocationCallbacks* p_Allocator
		)
		{
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_Instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr)
				func(p_Instance, p_DebugMessenger, p_Allocator);
		}
	}
}

#endif // __VIOLET_PLATFORM_VULKAN_EXTENSIONS_H_INCLUDED__