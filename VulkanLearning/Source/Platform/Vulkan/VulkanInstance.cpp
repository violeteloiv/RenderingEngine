#include "../Vulkan/VulkanInstance.h"

#include <stdexcept>

#include "../Vulkan/VulkanDebug.h"
#include "../Vulkan/VulkanExtensions.h"

namespace Violet
{
	VulkanInstance::VulkanInstance()
	{
		// Check if validation layers are available.
		if (ValidationLayersEnabled() && !CheckValidationLayerSupport())
			throw std::runtime_error("Validation Layers Requested But Not Available!");
		
		VkApplicationInfo appInfo = {};
		// Many structs in Vulkan require you to specify the 'structure' of the struct
		// in an sType parameter. Here we are specififying we are using an application
		// info struct.
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// Get required extensions.
		auto extensions = GetRequiredExtensions();

		// Feed those extensions into the vulkan create info struct.
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		// Include validation layer in createInfo.
		if (ValidationLayersEnabled())
		{
			auto validationLayers = GetValidationLayers();
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		// Debug info for vkInstance initialization and deconstruction.
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (ValidationLayersEnabled())
		{
			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = GetDebugMessengerCreateInfo();
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		// We now have all the information necessary to create the vulkan instance!
		if (vkCreateInstance(&createInfo, nullptr, &m_InstanceHandle) != VK_SUCCESS)
			throw std::runtime_error("Failed To Create Vulkan Instance!");
	}

	VulkanInstance::~VulkanInstance()
	{
		vkDestroyInstance(m_InstanceHandle, nullptr);
	}
}