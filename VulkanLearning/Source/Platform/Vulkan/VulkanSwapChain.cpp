#include "../Vulkan/VulkanSwapChain.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>

#ifdef VI_PLATFORM_WINDOWS
#include <GLFW/glfw3.h>
#endif

namespace Violet
{
	VulkanSwapChain::VulkanSwapChain(Ref<VulkanInstance> p_Instance, VkDevice p_Device, VkPhysicalDevice p_PhysicalDevice)
		: m_Device(p_Device), m_Instance(p_Instance), m_PhysicalDevice(p_PhysicalDevice)
	{
		SwapChainSupportDetails details = QuerySupportDetails(m_Instance, m_PhysicalDevice);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(details.SurfaceFormats);
		VkPresentModeKHR presentationMode = ChooseSwapPresentationMode(details.PresentationModes);
		VkExtent2D extent = ChooseSwapExtent(details.Capabilities);

		uint32_t imageCount = details.Capabilities.minImageCount + 1;
		if (details.Capabilities.maxImageCount > 0 && imageCount > details.Capabilities.maxImageCount)
			imageCount = details.Capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Instance->GetSurfaceHandle();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = VulkanPhysicalDevice::FindQueueFamilies(m_Instance, p_PhysicalDevice);
		uint32_t queueFamilyIndices[] = { indices.GraphicsFamily.value(), indices.GraphicsFamily.value() };

		if (indices.GraphicsFamily != indices.PresentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = details.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentationMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(p_Device, &createInfo, nullptr, &m_SwapChainHandle) != VK_SUCCESS)
			throw std::runtime_error("[ERROR] Failed To Create The SwapChain!");

		m_SwapChainExtent = extent;
		m_SwapChainFormat = surfaceFormat.format;

		vkGetSwapchainImagesKHR(p_Device, m_SwapChainHandle, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(p_Device, m_SwapChainHandle, &imageCount, m_SwapChainImages.data());
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		vkDestroySwapchainKHR(m_Device, m_SwapChainHandle, nullptr);
	}

	SwapChainSupportDetails VulkanSwapChain::QuerySupportDetails(Ref<VulkanInstance> p_Instance, VkPhysicalDevice p_PhysicalDevice)
	{
		SwapChainSupportDetails details;

		// get the surface's capabilities.
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p_PhysicalDevice, p_Instance->GetSurfaceHandle(), &details.Capabilities);

		// Get the formats.
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(p_PhysicalDevice, p_Instance->GetSurfaceHandle(), &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.SurfaceFormats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(p_PhysicalDevice, p_Instance->GetSurfaceHandle(), &formatCount, details.SurfaceFormats.data());
		}

		// Get the presentation modes.
		uint32_t presentationCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(p_PhysicalDevice, p_Instance->GetSurfaceHandle(), &presentationCount, nullptr);

		if (presentationCount != 0)
		{
			details.PresentationModes.resize(presentationCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(p_PhysicalDevice, p_Instance->GetSurfaceHandle(), &presentationCount, details.PresentationModes.data());
		}

		return details;
	}

	VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> p_AvailableFormats)
	{
		for (const auto& availableFormat : p_AvailableFormats)
		{
			// We are specifically trying to look for a specific format and color space, in this case:
			// - SRGB color space and having 32 bit colors in 8B, 8G, 8R, 8A order.
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}

		// If none of our options support this, just pick the first one.
		return p_AvailableFormats[0];
	}

	VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentationMode(std::vector<VkPresentModeKHR> p_AvailableModes)
	{
		for (const auto& availableMode : p_AvailableModes)
		{
			// We prefer mailbox mode due to it's nice tradeoffs in energy usage and lack of tearing.
			if (availableMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availableMode;
		}

		// This most accurately represents normal vertical sync found in games. This will be our default.
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& p_Capabilities)
	{
		if (p_Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return p_Capabilities.currentExtent;
		else
		{
			int width, height;
			glfwGetFramebufferSize((GLFWwindow*)m_Instance->GetWindow()->GetHandle(), &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, p_Capabilities.minImageExtent.width, p_Capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, p_Capabilities.minImageExtent.height, p_Capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}
}