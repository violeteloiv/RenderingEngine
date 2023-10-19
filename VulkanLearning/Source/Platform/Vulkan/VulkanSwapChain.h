#ifndef __VIOLET_PLATFORM_VULKAN_SWAP_CHAIN_H_INCLUDED__
#define __VIOLET_PLATFORM_VULKAN_SWAP_CHAIN_H_INCLUDED__

#include <vulkan/vulkan.h>

#include "../../Core/Pointer.h"

#include "../Vulkan/VulkanPhysicalDevice.h"
#include "../Vulkan/VulkanInstance.h"

#include <vector>

namespace Violet
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> SurfaceFormats;
		std::vector<VkPresentModeKHR> PresentationModes;
	};

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(Ref<VulkanInstance> p_Instance, VkDevice p_Device, VkPhysicalDevice p_PhysicalDevice);
		~VulkanSwapChain();
	public:
		static SwapChainSupportDetails QuerySupportDetails(Ref<VulkanInstance> p_Instance, VkPhysicalDevice p_PhysicalDevice);
	public:
		VkSwapchainKHR GetHandle() { return m_SwapChainHandle; }
		std::vector<VkImage> GetImages() { return m_SwapChainImages; }
		VkFormat GetFormat() { return m_SwapChainFormat; }
		VkExtent2D GetExtent() { return m_SwapChainExtent; }
	private:
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> p_AvailableFormats);
		VkPresentModeKHR ChooseSwapPresentationMode(std::vector<VkPresentModeKHR> p_AvailableModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& p_Capabilities);
	private:
		Ref<VulkanInstance> m_Instance;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_Device;
	private:
		VkSwapchainKHR m_SwapChainHandle;
		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_SwapChainFormat;
		VkExtent2D m_SwapChainExtent;
	};
}

#endif // __VIOLET_PLATFORM_VULKAN_SWAP_CHAIN_H_INCLUDED__