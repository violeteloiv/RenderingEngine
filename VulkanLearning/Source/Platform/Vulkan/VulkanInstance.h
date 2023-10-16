#ifndef __VIOLET_PLATFORM_VULKAN_INSTANCE_H_INCLUDED__
#define __VIOLET_PLATFORM_VULKAN_INSTANCE_H_INCLUDED__

#include <vulkan/vulkan.h>

#include "../../Core/Platform.h"

#ifdef VI_PLATFORM_WINDOWS
	#include "../Windows/WindowsExtensions.h"
#endif

#include <string>
#include <vector>

namespace Violet
{
	struct VulkanAppInfo
	{
		std::string AppName = "Application";
		int VersionMajor = 1;
		int VersionMinor = 0;
		int VersionRevision = 0;
	};

	class VulkanInstance
	{
	public:
		VulkanInstance(VulkanAppInfo& p_AppInfo);
		~VulkanInstance();
	public:
		VkInstance GetHandle() { return m_InstanceHandle; }
	public:
		void PopulateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void CreateDebugMessenger();
	private:
		std::vector<const char*> GetRequiredExtensions();
		// TODO: Move Extension Function Creation To Different File.
		VkResult CreateDebugUtilsMessengerEXT(
			const VkDebugUtilsMessengerCreateInfoEXT* p_CreateInfo,
			const VkAllocationCallbacks* p_Allocator,
			VkDebugUtilsMessengerEXT* p_DebugMessenger);
		void DestroyDebugUtilsMessengerEXT(
			VkDebugUtilsMessengerEXT p_DebugMessenger,
			const VkAllocationCallbacks* p_Allocator
		);
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT p_MessageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT p_MessageType,
			const VkDebugUtilsMessengerCallbackDataEXT* p_CallbackData,
			void* p_UserData
		);
	private:
		VkInstance m_InstanceHandle;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
	};
}

#endif // __VIOLET_PLATFORM_VULKAN_INSTANCE_H_INCLUDED__