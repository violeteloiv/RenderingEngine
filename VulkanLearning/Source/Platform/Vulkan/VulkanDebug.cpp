#include "../Vulkan/VulkanDebug.h"

#include "../Vulkan/VulkanExtensions.h"

#include <iostream>
#include <stdexcept>

namespace Violet
{
	void VulkanDebug::Init()
	{
		if (!CheckValidationLayerSupport())
			throw std::runtime_error("[ERROR] Validation Layers Requested, But Not Available!");
	}

	bool VulkanDebug::CheckValidationLayerSupport()
	{
		// Get the available validation layers.
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// Check!
		for (const char* layerName : VulkanExtensions::GetValidationLayers())
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

			if (!layerFound)
				return false;
		}

		return true;
	}
}