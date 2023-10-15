#ifndef __VIOLET_ENTRY_POINT_H_INCLUDED__
#define __VIOLET_ENTRY_POINT_H_INCLUDED__

#include "../Core/Platform.h"
#include "../Core/Debug.h"

#ifdef VI_PLATFORM_WINDOWS

#include "Application.h"

#include <iostream>

extern Violet::Ref<Violet::Application> Violet::CreateApplication();

int main(void)
{
	try {
#ifndef NDEBUG
		Violet::Ref<Violet::Debug> debug = Violet::Debug::Create();
		debug->Init();
#endif

		Violet::Ref<Violet::Application> app = Violet::CreateApplication();
		app->Run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}

#endif

#endif // __VIOLET_ENTRY_POINT_H_INCLUDED__