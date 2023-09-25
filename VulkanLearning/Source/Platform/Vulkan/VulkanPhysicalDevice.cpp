#include "../Vulkan/VulkanPhysicalDevice.h"

#include "../Vulkan/VulkanExtensions.h"
#include "../Vulkan/VulkanQueueFamily.h"

#include <map>
#include <stdexcept>

namespace Violet
{
	VulkanPhysicalDevice::VulkanPhysicalDevice(Ref<VulkanInstance> p_Instance, Ref<VulkanSurface> p_Surface)
		: m_Instance(p_Instance), m_Surface(p_Surface)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance->GetHandle(), &deviceCount, nullptr);

		// Error for if there are no devices.
		if (deviceCount == 0)
			throw std::runtime_error("Failed To Find GPUs With Vulkan Support!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance->GetHandle(), &deviceCount, devices.data());

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

	int VulkanPhysicalDevice::RateDeviceSuitability(VkPhysicalDevice p_PhysicalDevice)
	{
		// Query basic device properties and features.
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(p_PhysicalDevice, &deviceProperties);
		vkGetPhysicalDeviceFeatures(p_PhysicalDevice, &deviceFeatures);

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
		VulkanQueueFamily::Indices indices = VulkanQueueFamily::FindIndices(m_PhysicalDevice, m_Surface->GetHandle());
		bool extensionsSupported = CheckDeviceExtensionSupport(p_PhysicalDevice, m_DeviceExtensions);
		Renderer::SwapChain swapChain = Renderer::Renderer::GetInstance()->GetSwapChain();
		if (!indices.IsComplete() && !extensionsSupported && !swapChain.IsAdequate(extensionsSupported))
			return 0;

		return score;
	}

	VulkanPhysicalDevice::~VulkanPhysicalDevice()
	{

	}
}