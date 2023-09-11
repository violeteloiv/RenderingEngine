#ifndef __VIOLET_RENDERER_SWAP_CHAIN_H_INCLUDED__
#define __VIOLET_RENDERER_SWAP_CHAIN_H_INCLUDED__

#include <vector>
#include <vulkan/vulkan.h>

#include "../Core/Window.h"
#include "../Renderer/RenderPass.h"

namespace Renderer
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	struct SpecifiedSupportDetails
	{
		VkSurfaceFormatKHR Format;
		VkPresentModeKHR PresentMode;
		VkExtent2D Extent;
	};

	class SwapChain
	{
	public:
		SwapChain() {}
		SwapChain(VkDevice p_Device, VkPhysicalDevice p_PhysicalDevice, Core::Window* p_Window);
	public:
		void Shutdown();
	public:
		void GetSupportedDetails(VkPhysicalDevice p_Device, Core::Window* p_Window);
		void GetSelectedDetails(Core::Window* p_Window);
		std::vector<VkFramebuffer> GetFramebuffers();
		void GenerateFramebuffers(VkRenderPass p_RenderPass);
		bool IsAdequate(bool p_ExtensionsSupported);
		SpecifiedSupportDetails GetSpecifiedDetails();
	public:
		void CreateImageViews();
	public:
		VkSwapchainKHR GetHandle();
	private:
		VkDevice m_Device;
		VkSwapchainKHR m_SwapChainHandle;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;
	private:
		SwapChainSupportDetails m_Details;
		SpecifiedSupportDetails m_SelectedDetails;
	};
}

#endif // __VIOLET_RENDERER_SWAP_CHAIN_H_INCLUDED__