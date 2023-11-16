#include "../Pipelines/VulkanShader.h"

#include "../../../Core/Utils.h"

#include <iostream>

namespace Violet
{
	VulkanShader::VulkanShader(Ref<VulkanLogicalDevice> p_Device, const std::string& p_FilePath)
		: m_Device(p_Device), m_FilePath(p_FilePath)
	{
		// Read the shader file into a buffer.
		std::string fileContents = Utils::ReadFile(p_FilePath);

		// Split the buffer into it's shader components.
		m_ShaderDetails = ParseShader(fileContents);

		// Compile the shaders.
		m_ShaderDetails.VertexShaderModule = CompileShader(
			m_ShaderDetails.VertexString, shaderc_vertex_shader, true
		);
		m_ShaderDetails.FragmentShaderModule = CompileShader(
			m_ShaderDetails.FragmentString, shaderc_fragment_shader, true
		);

		// Create the shader stage in the pipeline info.
		VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
		vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderStageInfo.module = m_ShaderDetails.VertexShaderModule;
		vertexShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
		fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragmentShaderStageInfo.module = m_ShaderDetails.FragmentShaderModule;
		fragmentShaderStageInfo.pName = "main";

		m_ShaderDetails.ShaderStages.push_back(vertexShaderStageInfo);
		m_ShaderDetails.ShaderStages.push_back(fragmentShaderStageInfo);
	}

	VulkanShader::~VulkanShader()
	{
		vkDestroyShaderModule(m_Device->GetHandle(), m_ShaderDetails.VertexShaderModule, nullptr);
		vkDestroyShaderModule(m_Device->GetHandle(), m_ShaderDetails.FragmentShaderModule, nullptr);
	}

	VkShaderModule VulkanShader::CompileShader(std::string p_Source, shaderc_shader_kind p_Type, bool p_Optimize)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		if (p_Optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_size);

		shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
			p_Source, p_Type, "basic"
		);

		if (module.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			std::cerr << module.GetErrorMessage() << std::endl;
			throw std::runtime_error("[ERROR] Shader Module Failed To Compile!");
		}

		std::vector<uint32_t> code = { module.cbegin(), module.cend() };

		VkShaderModule shader;
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(m_Device->GetHandle(), &createInfo, nullptr, &shader) != VK_SUCCESS)
			throw std::runtime_error("[ERROR] Unable To Create Shader Module!");

		return shader;
	}

	ShaderDetails VulkanShader::ParseShader(std::string p_Source)
	{
		std::vector<std::string> lines = Violet::Utils::SplitString(p_Source, '\n');
		ShaderDetails details = GetChunks(lines);

		return details;
	}

	std::vector<ChunkStartEnd> VulkanShader::GetChunkLocations(std::vector<std::string> p_Lines)
	{
		std::vector<ChunkStartEnd> startEndPairs;

		size_t start = -1;
		size_t end = 0;
		for (size_t i = 0; i < p_Lines.size(); i++)
		{
			std::vector<std::string> words = Violet::Utils::SplitString(p_Lines[i], ' ');
			if (words[0].compare("#type") == 0)
			{
				if (start == -1)
					start = i;
				else
				{
					end = i - 1;
					startEndPairs.push_back({ start, end });
					start = i;
				}
			}
		}
		if (end < p_Lines.size())
		{
			end = p_Lines.size();
			startEndPairs.push_back({ start, end });
		}

		return startEndPairs;
	}

	ShaderDetails VulkanShader::GetChunks(std::vector<std::string> p_Lines)
	{
		ShaderDetails details;

		std::vector<ChunkStartEnd> pairs = GetChunkLocations(p_Lines);
		for (auto pair : pairs)
		{
			std::string buffer;
			std::string type = "";
			for (size_t i = pair.first; i < pair.second; i++)
			{
				std::vector<std::string> words = Violet::Utils::SplitString(p_Lines[i], ' ');
				if (words[0].compare("#type") == 0)
				{
					std::cout << words[1] << std::endl;
					type = words[1];
					continue;
				}
				buffer += (p_Lines[i] + "\n");
			}

			if (type.compare("vertex\r") == 0)
				details.VertexString = buffer;
			else if (type.compare("fragment\r") == 0)
				details.FragmentString = buffer;
		}

		return details;
	}
}