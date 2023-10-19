#include "../Vulkan/VulkanLogicalDevice.h"

#include "../Vulkan/VulkanExtensions.h"

#include <set>
#include <stdexcept>

namespace Violet
{
	VulkanLogicalDevice::VulkanLogicalDevice(Ref<VulkanInstance> p_Instance)
		: m_Instance(p_Instance)
	{
		// Setup the physical device.
		m_PhysicalDevice = CreateScope<VulkanPhysicalDevice>(m_Instance);
		VkPhysicalDevice physicalDevice = m_PhysicalDevice->GetHandle();

		// Specify the queues to be created.
		QueueFamilyIndices indices = VulkanPhysicalDevice::FindQueueFamilies(m_Instance, physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamily.value(), indices.PresentFamily.value()};

		// Define queue priorities.
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};

			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Specify the device features we want to use. We don't need anything special right now,
		// so we initialize everything to VK_FALSE.
		VkPhysicalDeviceFeatures deviceFeatures{};

		// Create the logical device.
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;

		auto deviceExtensions = VulkanExtensions::GetDeviceExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

#ifndef NDEBUG
		auto validationLayers = VulkanExtensions::GetValidationLayers();
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
#else
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
#endif

		// Finally create the device object.
		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_DeviceHandle) != VK_SUCCESS)
			throw std::runtime_error("[ERROR] Unable to create the vulkan logical device!");

		// Create the device queue.
		vkGetDeviceQueue(m_DeviceHandle, indices.GraphicsFamily.value(), 0, &m_GraphicsQueueHandle);
		vkGetDeviceQueue(m_DeviceHandle, indices.PresentFamily.value(), 0, &m_PresentQueueHandle);

		m_SwapChain = CreateRef<VulkanSwapChain>(m_Instance, m_DeviceHandle, m_PhysicalDevice->GetHandle());
		m_SwapChain->CreateImageViews();
	}

	VulkanLogicalDevice::~VulkanLogicalDevice()
	{
		// Manually delete swap chain bc it's being silly lol.
		m_SwapChain.reset();

		vkDestroyDevice(m_DeviceHandle, nullptr);
	}
}