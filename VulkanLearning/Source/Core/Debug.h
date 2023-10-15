#ifndef __VIOLET_CORE_DEBUG_H_INCLUDED__
#define __VIOLET_CORE_DEBUG_H_INCLUDED__

#include "../Core/Pointer.h"

namespace Violet
{
	class Debug
	{
	public:
		virtual ~Debug() = default;
	public:
		virtual void Init() = 0;
	public:
		static Ref<Debug> Create();
	};
}

#endif // __VIOLET_CORE_DEBUG_H_INCLUDED__