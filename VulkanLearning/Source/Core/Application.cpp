#include "../Core/Application.h"

#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>

#include "../Core/Debug.h"
#include "../Core/Extensions.h"
#include "../Core/QueueFamily.h" 

#include "../Renderer/Renderer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Core
{
	Application::Application()
		: m_PhysicalDevice(VK_NULL_HANDLE)
	{
		// We must initialize the GLFW window instance first.
		InitializeWindow();
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
		// This is our main program loop.
		MainLoop();
	}

	void Application::InitializeWindow()
	{
		// Initializes the GLFW library. First call in any program using a window.
		glfwInit();

		WindowProperties props = { 800, 600, "Vulkan" };
		m_Window = new Window(props);
	}

	void Application::InitializeVulkan()
	{
		// Create our VKInstance.
		CreateInstance();
		// Setup Debugging!!
		SetupDebugMessenger();

		// Create Surface For Connection To Window
		m_Window->CreateSurface(m_Instance);

		// Pick Graphics Card
		PickPhysicalDevice();
		// Create The Logical Device To Interface
		CreateLogicalDevice();

		// Setup Renderer
		Renderer::Renderer::GetInstance()->SetUp(m_Device, m_PhysicalDevice, m_Window);

		// Create the Sync Objects
		CreateSyncObjects();
	}

	void Application::MainLoop()
	{
		while (!glfwWindowShouldClose(m_Window->GetWindowPointer()))
		{
			// Check for events.
			glfwPollEvents();
			// Draw
			DrawFrame();
		}

		vkDeviceWaitIdle(m_Device);
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

		// Destroy DebugUtils
		if (Debug::EnableValidationLayers)
			Debug::DestroyDebugUtilsMessengerEXT(m_Instance, Debug::DebugMessenger, nullptr);

		// Destroy the Vulkan instance.
		vkDestroySurfaceKHR(m_Instance, m_Window->GetSurface(), nullptr);
		vkDestroyInstance(m_Instance, nullptr);

		// Delete our window instance.
		delete m_Window;

		// Close out GLFW.
		glfwTerminate();
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

	void Application::CreateInstance()
	{	
		// Check if validation layers are available.
		if (Debug::EnableValidationLayers && !CheckValidationLayerSupport())
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
		if (Debug::EnableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(Debug::ValidationLayers.size());
			createInfo.ppEnabledLayerNames = Debug::ValidationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		// Debug info for vkInstance initialization and deconstruction.
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (Debug::EnableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(Debug::ValidationLayers.size());
			createInfo.ppEnabledLayerNames = Debug::ValidationLayers.data();

			Debug::PopulateDebugMesengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		// We now have all the information necessary to create the vulkan instance!
		if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
			throw std::runtime_error("Failed To Create Vulkan Instance!");
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

	void Application::PickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

		// Error for if there are no devices.
		if (deviceCount == 0)
			throw std::runtime_error("Failed To Find GPUs With Vulkan Support!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

		// Use an ordered map to automatically sort candidates by increasing score.
		std::multimap<int, VkPhysicalDevice> candidates;

		// Find device
		for (const auto& device : devices)
		{
			int score = RateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}

		// Check if the best candidate is suitable at all
		if (candidates.rbegin()->first > 0)
			m_PhysicalDevice = candidates.rbegin()->second;
		else
			throw std::runtime_error("Failed To Find A Suitable GPU!");
		
		// Check if device exists!
		if (m_PhysicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("Failed To Find A Suitable GPU!");
	}

	int Application::RateDeviceSuitability(VkPhysicalDevice p_Device)
	{
		// Query basic device properties and features.
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(p_Device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(p_Device, &deviceFeatures);

		int score = 0;

		// Discrete GPUs have a significant performance advantage.
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 1000;

		// Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

		// Application can't function without geometry shaders.
		if (!deviceFeatures.geometryShader)
			return 0;

		// We don't want to use the device if it doesn't have the commands we want.
		Vulkan::QueueFamilyIndices indices = Vulkan::FindQueueFamilies(p_Device, m_Window->GetSurface());
		bool extensionsSupported = Extension::CheckDeviceExtensionSupport(p_Device);
		Renderer::SwapChain swapChain = Renderer::Renderer::GetInstance()->GetSwapChain();
		if (!indices.IsComplete() && !extensionsSupported && !swapChain.IsAdequate(extensionsSupported))
			return 0;

		return score;
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

	bool Application::CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		// Get the number of layers.
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		// Get the layers that are currently available.
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// Check to see if the layers are in.
		for (const char* layerName : Debug::ValidationLayers)
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

	std::vector<const char*> Application::GetRequiredExtensions()
	{
		// Since Vulkan is API Agnostic, we have to specify which window extensions
		// we are using. GLFW has a nice way of doing this.
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		if (!m_Window->VulkanSupportsGLFWExtensions(glfwExtensionCount, glfwExtensions))
			throw std::runtime_error("Vulkan Doesn't Support GLFW Extensions!");

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (Debug::EnableValidationLayers)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}

	void Application::SetupDebugMessenger()
	{
		if (!Debug::EnableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		Debug::PopulateDebugMesengerCreateInfo(createInfo);

		if (Debug::CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &Debug::DebugMessenger) != VK_SUCCESS)
			throw std::runtime_error("Failed To Set Up Debug Messenger!!");
	}
}