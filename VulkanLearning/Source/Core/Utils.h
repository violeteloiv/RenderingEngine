#ifndef __VIOLET_CORE_UTILS_H_INCLUDED__
#define __VIOLET_CORE_UTILS_H_INCLUDED__

#include <string>
#include <vector>

namespace Core
{
	namespace Utils
	{
		std::vector<char> ReadFile(const std::string& p_FileName);
	}
}

#endif // __VIOLET_CORE_UTILS_H_INCLUDED__