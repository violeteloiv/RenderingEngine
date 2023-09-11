#ifndef __VIOLET_CORE_DEBUG_H_INCLUDED__
#define __VIOLET_CORE_DEBUG_H_INCLUDED__

#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

namespace Core
{
	namespace Debug
	{
#ifdef NDEBUG
		const bool EnableValidationLayers = false;
#else
		const bool EnableValidationLayers = true;
#endif

		const std::vector<const char*> ValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		VkDebugUtilsMessengerEXT DebugMessenger;

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT p_MessageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT p_MessageType,
			const VkDebugUtilsMessengerCallbackDataEXT* p_CallbackData,
			void* p_UserData)
		{
			std::cerr << "Validation Layer: " << p_CallbackData->pMessage << std::endl;
			return VK_FALSE;
		}

		void PopulateDebugMesengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& p_CreateInfo)
		{
			p_CreateInfo = {};
			p_CreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			p_CreateInfo.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			p_CreateInfo.messageType = 
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			p_CreateInfo.pfnUserCallback = Debug::DebugCallback;
			p_CreateInfo.pUserData = nullptr; // Optional!
		}

		VkResult CreateDebugUtilsMessengerEXT(
			VkInstance p_Instance,
			const VkDebugUtilsMessengerCreateInfoEXT* p_CreateInfo,
			const VkAllocationCallbacks* p_Allocator,
			VkDebugUtilsMessengerEXT* p_DebugMessenger)
		{
			// Since DebugUtilsMessengerEXT is in an extension, we have to load it ourselves. The 
			// vkGetInstanceProcAddr function gets the address.
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_Instance, "vkCreateDebugUtilsMessengerEXT");
			if (func != nullptr)
				return func(p_Instance, p_CreateInfo, p_Allocator, p_DebugMessenger);
			else
				return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		void DestroyDebugUtilsMessengerEXT(
			VkInstance p_Instance,
			VkDebugUtilsMessengerEXT p_DebugMessenger,
			const VkAllocationCallbacks* p_Allocator)
		{
			// Same deal for DestroyDebugUtilsMessengerEXT
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_Instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr)
				func(p_Instance, p_DebugMessenger, p_Allocator);
		}
	}
}

#endif // __VIOLET_CORE_DEBUG_H_INCLUDED__