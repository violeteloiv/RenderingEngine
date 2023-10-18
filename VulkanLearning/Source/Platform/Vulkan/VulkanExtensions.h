#ifndef __VIOLET_PLATFORM_VULKAN_EXTENSIONS_H_INCLUDED__
#define __VIOLET_PLATFORM_VULKAN_EXTENSIONS_H_INCLUDED__

#include "../../Core/Platform.h"

#ifdef VI_PLATFORM_WINDOWS
#include "../Windows/WindowsExtensions.h"
#endif

#include <vulkan/vulkan.h>

#include <vector>

namespace Violet
{
	struct VulkanExtensions
	{
	public:
		static std::vector<const char*> GetValidationLayers() { return s_ValidationExtensions; }
		static std::vector<const char*> GetDeviceExtensions() { return s_DeviceExtensions; }
	public:
		static std::vector<const char*> s_ValidationExtensions;
		static std::vector<const char*> s_DeviceExtensions;
	};

	std::vector<const char*> GetRequiredExtensions();
	bool CheckDeviceExtensionSupport(VkPhysicalDevice p_PhysicalDevice);

	namespace DebugUtilsEXT
	{
		VkResult CreateDebugUtilsMessenger(
			VkInstance p_Instance,
			const VkDebugUtilsMessengerCreateInfoEXT* p_CreateInfo,
			const VkAllocationCallbacks* p_Allocator,
			VkDebugUtilsMessengerEXT* p_DebugMessenger);

		void DestroyDebugUtilsMessenger(
			VkInstance p_Instance,
			VkDebugUtilsMessengerEXT p_DebugMessenger,
			const VkAllocationCallbacks* p_Allocator
		);
	}
}

#endif // __VIOLET_PLATFORM_VULKAN_EXTENSIONS_H_INCLUDED__