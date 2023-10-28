#ifndef __VIOLET_PLATFORM_VULKAN_SHADER_H_INCLUDED__
#define __VIOLET_PLATFORM_VULKAN_SHADER_H_INCLUDED__

#include <string>
#include <vector>

namespace Violet
{
	typedef std::pair<size_t, size_t> ChunkStartEnd;

	struct ShaderDetails
	{
	public:
		std::string VertexString;
		std::string FragmentString;
	};

	class VulkanShader
	{
	public:
		VulkanShader(const std::string& p_FilePath);
		~VulkanShader();
	private:
		ShaderDetails ParseShader(std::string p_Source);
		std::vector<ChunkStartEnd> GetChunkLocations(std::vector<std::string> p_Lines);
		ShaderDetails GetChunks(std::vector<std::string> p_Lines);
	public:
		ShaderDetails m_ShaderDetails;
	private:
		const std::string& m_FilePath;
	};
}

#endif // __VIOLET_PLATFORM_VULKAN_SHADER_H_INCLUDED__