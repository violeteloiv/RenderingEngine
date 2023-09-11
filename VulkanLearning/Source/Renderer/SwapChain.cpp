#include "../Renderer/SwapChain.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>

#include "../Core/QueueFamily.h"

namespace Renderer
{
	SwapChain::SwapChain(VkDevice p_Device, VkPhysicalDevice p_PhysicalDevice, Core::Window* p_Window)
		: m_Device(p_Device)
	{
		GetSupportedDetails(p_PhysicalDevice, p_Window);
		GetSelectedDetails(p_Window);

		// It is recommended to use the minimum count + 1.
		uint32_t imageCount = m_Details.Capabilities.minImageCount + 1;
		// Make sure that we aren't going over the maximum. For the maximum,
		// 0 means that there is no maximum.
		if (m_Details.Capabilities.maxImageCount > 0 && imageCount > m_Details.Capabilities.maxImageCount)
			imageCount = m_Details.Capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = p_Window->GetSurface();
		// Supply Vulkan the information we gathered.
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = m_SelectedDetails.Format.format;
		createInfo.imageColorSpace = m_SelectedDetails.Format.colorSpace;
		createInfo.imageExtent = m_SelectedDetails.Extent;
		// This specifies the amount of layers each image consists of. It's
		// always 1 unless developing a stereoscopic 3D application.
		// TODO: Update for 3D applications maybe!!!
		createInfo.imageArrayLayers = 1;
		// This is how we specify the operations we will use on images.
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		// Specifying how we handle swap chain images used in multiple
		// queue families.
		Core::Vulkan::QueueFamilyIndices indices = Core::Vulkan::FindQueueFamilies(p_PhysicalDevice, p_Window->GetSurface());
		uint32_t queueFamilyIndices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

		// What happens if we have different queue families.
		if (indices.GraphicsFamily != indices.PresentFamily)
		{
			// Sharing is caring!
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			// No need to spend extra resources sharing!
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		// Specifies that images in the swap chain should maintain their
		// current transformation.
		createInfo.preTransform = m_Details.Capabilities.currentTransform;

		// Essentially ignores alpha when it comes to blending between windows.
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		// If clipped is true that means we don't care about
		// the color of pixels that are obscured.
		createInfo.presentMode = m_SelectedDetails.PresentMode;
		createInfo.clipped = VK_TRUE;

		// TODO: When resizing, the swap chain becomes unreliable so we have
		// to recreate it. This will be covered later.
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChainHandle) != VK_SUCCESS)
			throw std::runtime_error("Failed To Create Swap Chain!");

		// Get the swap chain images.
		vkGetSwapchainImagesKHR(m_Device, m_SwapChainHandle, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device, m_SwapChainHandle, &imageCount, m_SwapChainImages.data());

		// Create The Image Views For The SwapChain
		CreateImageViews();
	}

	void SwapChain::GetSupportedDetails(VkPhysicalDevice p_PhysicalDevice, Core::Window* p_Window)
	{
		SwapChainSupportDetails details;

		VkSurfaceKHR surface = p_Window->GetSurface();

		// Get the basic surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p_PhysicalDevice, surface, &details.Capabilities);

		// Get the formats we can use.
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(p_PhysicalDevice, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(p_PhysicalDevice, surface, &formatCount, details.Formats.data());
		}

		// Get the presentation modes.
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(p_PhysicalDevice, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.PresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(p_PhysicalDevice, surface, &presentModeCount, details.PresentModes.data());
		}

		m_Details = details;
	}

	void SwapChain::Shutdown()
	{
		// Destroy framebuffers.
		for (auto framebuffer : m_SwapChainFramebuffers)
			vkDestroyFramebuffer(m_Device, framebuffer, nullptr);

		// Destroy the image views.
		for (auto imageView : m_SwapChainImageViews)
			vkDestroyImageView(m_Device, imageView, nullptr);

		// Destroy swap chain handle.
		vkDestroySwapchainKHR(m_Device, m_SwapChainHandle, nullptr);
	}

	void SwapChain::GetSelectedDetails(Core::Window* p_Window)
	{
		// Select the optimal surface format. For us this will be SRGB if it's
		// available since it's kind of the default useful one.
		m_SelectedDetails.Format = m_Details.Formats[0];
		for (const auto& availableFormat : m_Details.Formats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				m_SelectedDetails.Format = availableFormat;
		}

		// There are four presentation modes available in SwapChains for Vulkan. It
		// represents the actual conditions for showing images to the screen.
		//
		// VK_PRESENT_MODE_IMMEDIATE_KHR: Images submitted by the application are
		// transferred to the screen right away, might result in tearing.
		// VK_PRESENT_MODE_FIFO_KHR: The swap chain acts as a queue whre the display
		// takes an image from the front of the queue when the display is refreshed.
		// The application inserts rendered images to the back of the queue. This
		// is the most similar to vertical sync found in modern games.
		// VK_PRESENT_MODE_FIFO_RELAXED_KHR: This differs from the previous in the
		// situation where the application is late. If the application is late in
		// rendering, it will transfer images automatically like in the first option,
		// possibly resulting in tearing.
		// VK_PRESENT_MODE_MAILBOX_KHR: Instead of blocking the application when the
		// queue is full, the images that are queued are replaced with the newer ones.
		// This is commonly known as "triple buffering".

		// Only the second option is guaranteed to be available, so we use that by default,
		// however the fourth option is optimal so we look for it.
		m_SelectedDetails.PresentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto& availablePresentMode : m_Details.PresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				m_SelectedDetails.PresentMode = availablePresentMode;
		}

		// We need to specify the extent, or the resolution. This is in pixels for Vulkan,
		// while the width and height we specified is in screen coordinates. We must retrieve
		// this information from GLFW.
		if (m_Details.Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			m_SelectedDetails.Extent = m_Details.Capabilities.currentExtent;
		}
		else
		{
			// Get the width, and height.
			int width, height;
			glfwGetFramebufferSize(p_Window->GetWindowPointer(), &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			// Clamp the width between the min and max values.
			actualExtent.width = std::clamp(
				actualExtent.width,
				m_Details.Capabilities.minImageExtent.width,
				m_Details.Capabilities.maxImageExtent.width
			);

			// Clamp the height between the min and max values.
			actualExtent.height = std::clamp(
				actualExtent.height,
				m_Details.Capabilities.minImageExtent.height,
				m_Details.Capabilities.maxImageExtent.height
			);

			m_SelectedDetails.Extent = actualExtent;
		}
	}

	std::vector<VkFramebuffer> SwapChain::GetFramebuffers()
	{
		return m_SwapChainFramebuffers;
	}

	void SwapChain::GenerateFramebuffers(VkRenderPass p_RenderPass)
	{
		m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

		// Create all the framebuffers.
		for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
		{
			VkImageView attachments[] = {
				m_SwapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = p_RenderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = m_SelectedDetails.Extent.width;
			framebufferInfo.height = m_SelectedDetails.Extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed To Create Framebuffer!");
		}
	}

	bool SwapChain::IsAdequate(bool p_ExtensionsSupported)
	{
		if (p_ExtensionsSupported)
		{
			return !m_Details.Formats.empty() && !m_Details.PresentModes.empty();
		}

		return false;
	}

	SpecifiedSupportDetails SwapChain::GetSpecifiedDetails()
	{
		return m_SelectedDetails;
	}

	VkSwapchainKHR SwapChain::GetHandle()
	{
		return m_SwapChainHandle;
	}

	void SwapChain::CreateImageViews()
	{
		// Resize Number Of Swap Chain Image Views
		m_SwapChainImageViews.resize(m_SwapChainImages.size());

		for (size_t i = 0; i < m_SwapChainImages.size(); i++)
		{
			// Create the image views!
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_SwapChainImages[i];

			// Specify how the image data should be interpreted.
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_SelectedDetails.Format.format;

			// We can swizzle color channels around. We will keep them
			// default for now.
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			// The subresourceRange field describes what the image's
			// purpose is and which part of the image should be
			// accessed. If working with a stereographic 3D application,
			// we would create a swap chain with multiple layers using
			// multiple image views for each image views.
			// TODO: Relook at for 3D stereographic images.
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			// Create the image using the info:
			if (vkCreateImageView(m_Device, &createInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed To Create Image View!");
		}
	}
}