#include "../Vulkan/VulkanPhysicalDevice.h"

#include "../Vulkan/VulkanExtensions.h"

#include <map>
#include <stdexcept>
#include <vector>

namespace Violet
{
	VulkanPhysicalDevice::VulkanPhysicalDevice(Ref<VulkanInstance> p_Instance)
		: m_Instance(p_Instance)
	{
		uint32_t deviceCount;
		vkEnumeratePhysicalDevices(m_Instance->GetHandle(), &deviceCount, nullptr);

		if (deviceCount == 0)
			throw std::runtime_error("[ERROR] Vulkan Could Not Identify Compatible GPUs");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance->GetHandle(), &deviceCount, devices.data());

		std::multimap<uint32_t, VkPhysicalDevice> candidates;

		for (const auto& device : devices)
		{
			int score = RatePhysicalDevice(device);
			candidates.insert(std::make_pair(score, device));
		}

		if (candidates.rbegin()->first > 0)
			m_PhysicalDeviceHandle = candidates.rbegin()->second;
		else
			throw std::runtime_error("[ERROR] Unable To Find Suitable Physical Device.");
	}

	uint32_t VulkanPhysicalDevice::RatePhysicalDevice(VkPhysicalDevice p_PhysicalDevice)
	{
		// Get device properties and features.
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(p_PhysicalDevice, &props);
		VkPhysicalDeviceFeatures feats;
		vkGetPhysicalDeviceFeatures(p_PhysicalDevice, &feats);
		
		uint32_t score = 0;

		// Discrete GPUs have big performance advantage.
		if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 1000;

		// Maximum number of textures effects performance quality.
		score += props.limits.maxImageDimension2D;

		// Application can't work without geometry shaders.
		if (!feats.geometryShader)
			score = 0;

		// We require our device to have a specific set of queues including:
		// - Graphics Queue
		// - Presentation Queue
		QueueFamilyIndices indices = FindQueueFamilies(p_PhysicalDevice);
		if (!indices.IsComplete())
			score = 0;

		// We also require certain extensions to be supported included:
		// - Swap Chain Extension
		bool extensionsSupported = CheckDeviceExtensionSupport(p_PhysicalDevice);
		if (!extensionsSupported)
			score = 0;

		// Our swap chain must also be adequate.
		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			SwapChainSupportDetails details = QuerySwapChainSupport(p_PhysicalDevice);
			swapChainAdequate = !details.SurfaceFormats.empty() && !details.PresentationModes.empty();
		}

		if (!swapChainAdequate)
			score = 0;

		return score;
	}

	QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies(VkPhysicalDevice p_PhysicalDevice)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(p_PhysicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilyProps(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(p_PhysicalDevice, &queueFamilyCount, queueFamilyProps.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilyProps)
		{
			// Assign the VK_QUEUE_GRAPHICS_BIT index.
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.GraphicsFamily = i;

			// Check for presentation support.
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(p_PhysicalDevice, i, m_Instance->GetSurfaceHandle(), &presentSupport);
			if (presentSupport)
				indices.PresentFamily = i;

			if (indices.IsComplete())
				break;

			i++;
		}

		return indices;
	}

	SwapChainSupportDetails VulkanPhysicalDevice::QuerySwapChainSupport(VkPhysicalDevice p_PhysicalDevice)
	{
		SwapChainSupportDetails details;

		// get the surface's capabilities.
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p_PhysicalDevice, m_Instance->GetSurfaceHandle(), &details.Capabilities);

		// Get the formats.
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(p_PhysicalDevice, m_Instance->GetSurfaceHandle(), &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.SurfaceFormats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(p_PhysicalDevice, m_Instance->GetSurfaceHandle(), &formatCount, details.SurfaceFormats.data());
		}

		// Get the presentation modes.
		uint32_t presentationCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(p_PhysicalDevice, m_Instance->GetSurfaceHandle(), &presentationCount, nullptr);

		if (presentationCount != 0)
		{
			details.PresentationModes.resize(presentationCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(p_PhysicalDevice, m_Instance->GetSurfaceHandle(), &presentationCount, details.PresentationModes.data());
		}

		return details;
	}
}