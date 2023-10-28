#include "../Pipelines/VulkanShader.h"

#include "../../../Core/Utils.h"

#include <iostream>

namespace Violet
{
	VulkanShader::VulkanShader(const std::string& p_FilePath)
		: m_FilePath(p_FilePath)
	{
		// Read the shader file into a buffer.
		std::string fileContents = Utils::ReadFile(p_FilePath);

		// Split the buffer into it's shader components.
		m_ShaderDetails = ParseShader(fileContents);

		

		// Compile the shaders.
	}

	VulkanShader::~VulkanShader()
	{

	}

	ShaderDetails VulkanShader::ParseShader(std::string p_Source)
	{
		std::vector<std::string> lines = Violet::Utils::SplitString(p_Source, '\n');
		ShaderDetails details = GetChunks(lines);

		std::cout << "Vertex Shader: " << std::endl;
		std::cout << details.VertexString << std::endl;
		std::cout << "Fragment Shader: " << std::endl;
		std::cout << details.FragmentString << std::endl;

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