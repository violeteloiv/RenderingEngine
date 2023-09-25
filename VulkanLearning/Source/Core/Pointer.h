#ifndef __VIOLET_CORE_POINTER_H_INCLUDED__
#define __VIOLET_CORE_POINTER_H_INCLUDED__

#include <memory>

namespace Violet
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... p_Args)
	{
		return std::make_unique<T>(std::forward<Args>(p_Args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... p_Args)
	{
		return std::make_shared<T>(std::forward<Args>(p_Args)...);
	}
}

#endif // __VIOLET_CORE_POINTER_H_INCLUDED__