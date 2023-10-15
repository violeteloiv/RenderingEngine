#include "../Vulkan/VulkanInstance.h"

#include "../Vulkan/VulkanDebug.h"

#include <iostream>
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
		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
#else
		createInfo.enabledExtensionCount = 0;
		createInfo.ppEnabledExtensionNames = nullptr;
#endif

#ifndef NDEBUG
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

		auto validationLayers = VulkanDebug::GetValidationLayers();
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessenger(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#else
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
#endif

		// Create the instance.
		if (vkCreateInstance(&createInfo, nullptr, &m_InstanceHandle) != VK_SUCCESS)
			throw std::runtime_error("[ERROR] VulkanInstance - Failed To Create Instance.");
	}

	VulkanInstance::~VulkanInstance()
	{
#ifndef NDEBUG
		DestroyDebugUtilsMessengerEXT(m_DebugMessenger, nullptr);
#endif

		vkDestroyInstance(m_InstanceHandle, nullptr);
	}

	void VulkanInstance::PopulateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
		createInfo.pUserData = nullptr; // Optional.
	}

	void VulkanInstance::CreateDebugMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		PopulateDebugMessenger(createInfo);

		if (CreateDebugUtilsMessengerEXT(&createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
			throw std::runtime_error("[ERROR] Debug Messenger Not Created Successfully!");
	}

	std::vector<const char*> VulkanInstance::GetRequiredExtensions()
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

	VkResult VulkanInstance::CreateDebugUtilsMessengerEXT(
		const VkDebugUtilsMessengerCreateInfoEXT* p_CreateInfo,
		const VkAllocationCallbacks* p_Allocator,
		VkDebugUtilsMessengerEXT* p_DebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_InstanceHandle, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
			return func(m_InstanceHandle, p_CreateInfo, p_Allocator, p_DebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void VulkanInstance::DestroyDebugUtilsMessengerEXT(
		VkDebugUtilsMessengerEXT p_DebugMessenger,
		const VkAllocationCallbacks* p_Allocator
	)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_InstanceHandle, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(m_InstanceHandle, p_DebugMessenger, p_Allocator);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT p_MessageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT p_MessageType,
		const VkDebugUtilsMessengerCallbackDataEXT* p_CallbackData,
		void* p_UserData
	)
	{
		std::cerr << "Validation Layer: " << p_CallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
}