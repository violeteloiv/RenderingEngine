#include "../Core/Utils.h"

#include <fstream>
#include <stdexcept>

namespace Violet
{
	namespace Utils
	{
		std::string ReadTextFile(const std::string& p_FileName)
		{
			std::vector<char> buffer = ReadBinaryFile(p_FileName);

			std::string str = "";
			for (auto character : buffer)
				str += character;

			return str;
		}

		std::vector<char> ReadBinaryFile(const std::string& p_FileName)
		{
			std::ifstream file(p_FileName, std::ios::ate | std::ios::binary);

			if (!file.is_open())
				throw std::runtime_error("Failed To Open File!");

			// Get the file size.
			size_t fileSize = (size_t)file.tellg();
			std::vector<char> buffer(fileSize);

			// Go back to the beginning.
			file.seekg(0);
			// Read all of the bytes at once.
			file.read(buffer.data(), fileSize);

			// Clode file.
			file.close();

			return buffer;

		}

		void WriteFile(const std::string& p_FilePath, std::string& p_Data)
		{
			// Write to file.
			std::ofstream file(p_FilePath);
			file << p_Data;
			file.close();
		}

		std::vector<std::string> SplitString(std::string& p_Source, char p_Delimeter)
		{
			std::vector<std::string> strings;

			std::string::size_type pos = 0;
			std::string::size_type prev = 0;
			while ((pos = p_Source.find(p_Delimeter, prev)) != std::string::npos)
			{
				strings.push_back(p_Source.substr(prev, pos - prev));
				prev = pos + 1;
			}

			strings.push_back(p_Source.substr(prev));

			return strings;
		}
	}
}