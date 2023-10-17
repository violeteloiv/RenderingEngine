#include "../Vulkan/VulkanLogicalDevice.h"

#include "../Vulkan/VulkanDebug.h"

#include <stdexcept>

namespace Violet
{
	VulkanLogicalDevice::VulkanLogicalDevice(Ref<VulkanInstance> p_Instance)
		: m_Instance(p_Instance)
	{
		// Setup the physical device.
		m_PhysicalDevice = CreateScope<VulkanPhysicalDevice>(m_Instance);

		// Specify the queues to be created.
		QueueFamilyIndices indices = m_PhysicalDevice->FindQueueFamilies(m_PhysicalDevice->GetHandle());

		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.GraphicsFamily.value();
		queueCreateInfo.queueCount = 1;

		// Define the queue priorities.
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		// Specify the device features we want to use. We don't need anything special right now,
		// so we initialize everything to VK_FALSE.
		VkPhysicalDeviceFeatures deviceFeatures{};

		// Create the logical device.
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = 0;
		createInfo.ppEnabledExtensionNames = nullptr;

#ifndef NDEBUG
		auto validationLayers = VulkanDebug::GetValidationLayers();
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
#else
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
#endif

		// Finally create the device object.
		if (vkCreateDevice(m_PhysicalDevice->GetHandle(), &createInfo, nullptr, &m_DeviceHandle) != VK_SUCCESS)
			throw std::runtime_error("[ERROR] Unable to create the vulkan logical device!");

		// Create the device queue.
		vkGetDeviceQueue(m_DeviceHandle, indices.GraphicsFamily.value(), 0, &m_GraphicsQueueHandle);
	}

	VulkanLogicalDevice::~VulkanLogicalDevice()
	{
		vkDestroyDevice(m_DeviceHandle, nullptr);
	}
}