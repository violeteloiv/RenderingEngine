#include "../Core/Application.h"

#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>

#include "../Core/Extensions.h"
#include "../Core/QueueFamily.h" 

#include "../Renderer/Renderer.h"
#include "../Renderer/RenderCommand.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Core
{
	Application::Application()
		: m_PhysicalDevice(VK_NULL_HANDLE)
	{
		m_Window = Violet::CreateScope<Violet::Window>();
		Violet::RenderCommand::Init(m_Window);

		// We can now initialize Vulkan!
		InitializeVulkan();
	}

	Application::~Application()
	{
		// We have to clean up everything that we make in our vulkan program!
		CleanUp();
	}

	void Application::Run()
	{
		while (!m_Window->ShouldClose())
		{
			// Update Window
			m_Window->OnUpdate();

			// Draw
			DrawFrame();
		}

		vkDeviceWaitIdle(m_Device);
	}

	void Application::InitializeVulkan()
	{
		// Pick Graphics Card
		PickPhysicalDevice();
		// Create The Logical Device To Interface
		CreateLogicalDevice();

		// Setup Renderer
		Renderer::Renderer::GetInstance()->SetUp(m_Device, m_PhysicalDevice, m_Window);

		// Create the Sync Objects
		CreateSyncObjects();
	}

	void Application::CleanUp()
	{
		// Destroy Semaphores
		vkDestroySemaphore(m_Device, m_ImageAvailableSemaphore, nullptr);
		vkDestroySemaphore(m_Device, m_RenderFinishedSemaphore, nullptr);
		vkDestroyFence(m_Device, m_InFlightFence, nullptr);

		// Shutdown Renderer
		Renderer::Renderer::GetInstance()->Shutdown(m_Device);

		// Destroy Device
		vkDestroyDevice(m_Device, nullptr);		
	}

	void Application::DrawFrame()
	{
		// Wait for the previous frame.
		vkWaitForFences(m_Device, 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
		// Reset the fences.
		vkResetFences(m_Device, 1, &m_InFlightFence);

		// Get the image index from the swap change.
		uint32_t imageIndex;
		vkAcquireNextImageKHR(m_Device, Renderer::Renderer::GetInstance()->GetSwapChain().GetHandle(), UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		// Command Buffer Time
		vkResetCommandBuffer(Renderer::Renderer::GetInstance()->GetRenderCommand().GetHandle(), imageIndex);

		// Record the buffer
		VkCommandBuffer commandBuffer = Renderer::Renderer::GetInstance()->RecordCommandBuffer(imageIndex);

		// Create the submit info for the renderer.
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		// Submit the single command buffer.
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		
		VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		// Finally submit the buffer.
		if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFence) != VK_SUCCESS)
			throw std::runtime_error("Failed To Submit Draw Command Buffer!");

		// Now that we've submitted, we actually have to present.
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { Renderer::Renderer::GetInstance()->GetSwapChain().GetHandle() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;
		
		vkQueuePresentKHR(m_PresentQueue, &presentInfo);
	}

	void Application::CreateLogicalDevice()
	{
		Vulkan::QueueFamilyIndices indices = Vulkan::FindQueueFamilies(m_PhysicalDevice, m_Window->GetSurface());

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		// We use a set to generate all possible combinations.
		std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

		// Use queue prioties.
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			// Add the info to the list
			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Specify devices features to use.
		// TODO: Add features that we want to use.
		VkPhysicalDeviceFeatures deviceFeatures{};

		// Creating the logical device.
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		// Add points to queue and device feature structs.
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;

		// Label extensions enabled.
		createInfo.enabledExtensionCount = static_cast<uint32_t>(Extension::DeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = Extension::DeviceExtensions.data();

		// Label validation layers used.
		if (Debug::EnableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(Debug::ValidationLayers.size());
			createInfo.ppEnabledLayerNames = Debug::ValidationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		// Create device with the specified info!
		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
			throw std::runtime_error("Failed To Create Logical Device!");

		// Get handle to graphics queue.
		vkGetDeviceQueue(m_Device, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
		// Get handle to present queue.
		vkGetDeviceQueue(m_Device, indices.PresentFamily.value(), 0, &m_PresentQueue);
	}

	void Application::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore) ||
			vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore) ||
			vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlightFence) != VK_SUCCESS)
			std::runtime_error("Failed To Create Semaphores!");
	}
}