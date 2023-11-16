#ifndef __VIOLET_PLATFORM_VULKAN_SHADER_H_INCLUDED__
#define __VIOLET_PLATFORM_VULKAN_SHADER_H_INCLUDED__

#include "../../Vulkan/VulkanLogicalDevice.h"

#include <string>
#include <vector>

#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.h>

namespace Violet
{
	typedef std::pair<size_t, size_t> ChunkStartEnd;

	struct ShaderDetails
	{
	public:
		std::string VertexString;
		std::string FragmentString;
	public:
		VkShaderModule VertexShaderModule;
		VkShaderModule FragmentShaderModule;
	public:
		std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;
	};

	class VulkanShader
	{
	public:
		VulkanShader(Ref<VulkanLogicalDevice> p_Device, const std::string& p_FilePath);
		~VulkanShader();
	public:
		ShaderDetails GetShaderDetails() { return m_ShaderDetails; }
	private:
		VkShaderModule CompileShader(
			std::string p_Source,
			shaderc_shader_kind p_Type,
			bool p_Optimize = false
		);
	private:
		ShaderDetails ParseShader(std::string p_Source);
		std::vector<ChunkStartEnd> GetChunkLocations(std::vector<std::string> p_Lines);
		ShaderDetails GetChunks(std::vector<std::string> p_Lines);
	private:
		ShaderDetails m_ShaderDetails;
		const std::string& m_FilePath;
		Ref<VulkanLogicalDevice> m_Device;
	};
}

#endif // __VIOLET_PLATFORM_VULKAN_SHADER_H_INCLUDED__