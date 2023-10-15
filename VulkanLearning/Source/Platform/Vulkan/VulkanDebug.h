#ifndef __VIOLET_PLATFORM_VULKAN_DEBUG_H_INCLUDED__
#define __VIOLET_PLATFORM_VULKAN_DEBUG_H_INCLUDED__

#include "../../Core/Debug.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Violet
{
	class VulkanDebug : public Debug
	{
	public:
		VulkanDebug() = default;
		virtual ~VulkanDebug() = default;
	public:
		virtual void Init();
	public:
		static const std::vector<const char*> GetValidationLayers() { return s_ValidationLayers; }
	private:
		bool CheckValidationLayerSupport();
	private:
		static const std::vector<const char*> s_ValidationLayers;
	};
}

#endif // __VIOLET_PLATFORM_VULKAN_DEBUG_H_INCLUDED__