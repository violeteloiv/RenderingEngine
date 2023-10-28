#ifndef __VIOLET_CORE_UTILS_H_INCLUDED__
#define __VIOLET_CORE_UTILS_H_INCLUDED__

#include <string>
#include <vector>

namespace Violet
{
	namespace Utils
	{
		std::string ReadFile(const std::string& p_FileName);
		std::vector<std::string> SplitString(std::string& p_Source, char p_Delimeter);
	}
}

#endif // __VIOLET_CORE_UTILS_H_INCLUDED__