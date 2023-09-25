#include "../Core/Window.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

#ifdef VI_PLATFORM_WINDOWS
	#include "../Platform/Windows/WindowsWindow.h"
#endif

namespace Violet
{
	Scope<Window> Window::Create(const WindowProperties& p_Properties)
	{
	#ifdef VI_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(p_Properties);
	#else
		std::runtime_error("Unknown Platform!");
	#endif
	}
}