#include "../Vulkan/VulkanInstance.h"

#include "../Vulkan/VulkanDebug.h"
#include "../Vulkan/VulkanExtensions.h"

#ifdef VI_PLATFORM_WINDOWS
#include <GLFW/glfw3.h>
#endif

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

		auto validationLayers = VulkanExtensions::GetValidationLayers();
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
		DebugUtilsEXT::DestroyDebugUtilsMessenger(m_InstanceHandle, m_DebugMessenger, nullptr);
#endif
		
		vkDestroySurfaceKHR(m_InstanceHandle, m_WindowSurface, nullptr);
		vkDestroyInstance(m_InstanceHandle, nullptr);
	}

	void VulkanInstance::PopulateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
			// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
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

		if (DebugUtilsEXT::CreateDebugUtilsMessenger(m_InstanceHandle, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
			throw std::runtime_error("[ERROR] Debug Messenger Not Created Successfully!");
	}

	void VulkanInstance::InstantiateSurface(Ref<Window> p_Window)
	{
		if (glfwCreateWindowSurface(m_InstanceHandle, (GLFWwindow*)p_Window->GetHandle(), nullptr, &m_WindowSurface) != VK_SUCCESS)
			throw std::runtime_error("[ERROR] Unable To Create Window Surface!");
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