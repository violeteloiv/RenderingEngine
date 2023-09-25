#include "../Vulkan/VulkanDebug.h"

#include <vulkan/vulkan.h>

namespace Violet
{
	VulkanDebugMessenger::VulkanDebugMessenger(Ref<VulkanInstance> p_Instance)
		: m_Instance(p_Instance)
	{
		if (!ValidationLayersEnabled()) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo = GetDebugMessengerCreateInfo();

		if (CreateDebugUtilsMessengerEXT(p_Instance->GetHandle(), &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
			throw std::runtime_error("Failed To Set Up Debug Messenger!!");

	}

	VulkanDebugMessenger::~VulkanDebugMessenger()
	{
		DestroyDebugUtilsMessengerEXT(m_Instance->GetHandle(), m_DebugMessenger, nullptr);
	}

	bool ValidationLayersEnabled()
	{
#ifdef NDEBUG
		return false;
#else
		return true;
#endif
	}

	std::vector<const char*> GetValidationLayers()
	{
		return { 
			"VK_LAYER_KHRONOS_validation" 
		};
	}

	bool CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		// Get the number of layers.
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		// Get the layers that are currently available.
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// Check to see if the layers are in.
		for (const char* layerName : GetValidationLayers())
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound) return false;
		}

		return true;
	}
}