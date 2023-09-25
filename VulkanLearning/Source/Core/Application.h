#ifndef __VIOLET_CORE_APPLICATION_H__
#define __VIOLET_CORE_APPLICATION_H__

#include "../Core/Window.h"

#include <vector>

namespace Violet
{
	class Application
	{
	public:
		Application();
		virtual ~Application() = default;
	public:
		void Run();
	private:
		Ref<Window> m_Window;
	};

	Application* CreateApplication();
}

#endif // __VIOLET_CORE_APPLICATION_H__