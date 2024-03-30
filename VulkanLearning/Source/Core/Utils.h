#ifndef __VIOLET_CORE_UTILS_H_INCLUDED__
#define __VIOLET_CORE_UTILS_H_INCLUDED__

#include <fstream>
#include <string>
#include <vector>

namespace Violet
{
	namespace Utils
	{
		std::string ReadTextFile(const std::string& p_FileName);
		std::vector<char> ReadBinaryFile(const std::string& p_FileName);
		void WriteFile(const std::string& p_FileName, std::string& p_Data);
		std::vector<std::string> SplitString(std::string& p_Source, char p_Delimeter);
	}
}

#endif // __VIOLET_CORE_UTILS_H_INCLUDED__