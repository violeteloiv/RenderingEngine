#include "../Vulkan/VulkanLogicalDevice.h"

#include "../Vulkan/VulkanDebug.h"

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
		QueueFamilyIndices indices = m_PhysicalDevice->FindQueueFamilies(physicalDevice);

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
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
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
		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_DeviceHandle) != VK_SUCCESS)
			throw std::runtime_error("[ERROR] Unable to create the vulkan logical device!");

		// Create the device queue.
		vkGetDeviceQueue(m_DeviceHandle, indices.GraphicsFamily.value(), 0, &m_GraphicsQueueHandle);
		vkGetDeviceQueue(m_DeviceHandle, indices.PresentFamily.value(), 0, &m_PresentQueueHandle);
	}

	VulkanLogicalDevice::~VulkanLogicalDevice()
	{
		vkDestroyDevice(m_DeviceHandle, nullptr);
	}
}