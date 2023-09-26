#ifndef __VIOLET_PLATFORM_VULKAN_INSTANCE_H_INCLUDED__
#define __VIOLET_PLATFORM_VULKAN_INSTANCE_H_INCLUDED__

#include <vulkan/vulkan.h>

#include "../../Core/Platform.h"

#ifdef VI_PLATFORM_WINDOWS
	#include "../Windows/WindowsExtensions.h"
#endif

#include <string>

namespace Violet
{
	struct VulkanAppInfo
	{
		std::string AppName = "Application";
		int VersionMajor = 1;
		int VersionMinor = 0;
		int VersionRevision = 0;
	};

	class VulkanInstance
	{
	public:
		VulkanInstance(VulkanAppInfo& p_AppInfo);
		~VulkanInstance();
	public:
		VkInstance GetHandle() { return m_InstanceHandle; }
	private:
		VkInstance m_InstanceHandle;
	};
}

#endif // __VIOLET_PLATFORM_VULKAN_INSTANCE_H_INCLUDED__