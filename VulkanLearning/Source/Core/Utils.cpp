#include "../Core/Utils.h"

#include <fstream>
#include <stdexcept>

namespace Core
{
	namespace Utils
	{
		std::vector<char> ReadFile(const std::string& p_FileName)
		{
			// Read the file as a binary file and set the cursor pointer to the end of file.
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

			// Close file.
			file.close();

			return buffer;
		}
	}
}