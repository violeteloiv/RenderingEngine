#ifndef __VIOLET_CORE_WINDOW_H_INCLUDED__
#define __VIOLET_CORE_WINDOW_H_INCLUDED__

#include <cstdlib>
#include <string>

#include "../Core/Platform.h"
#include "../Core/Pointer.h"

namespace Violet
{
	struct WindowProperties
	{
	public:
		const std::string& Name;
		const uint32_t Width;
		const uint32_t Height;
	public:
		WindowProperties(const std::string& p_Name = "Vulkan", uint32_t p_Width = 800, uint32_t p_Height = 600)
			: Name(p_Name), Width(p_Width), Height(p_Height) { }
	};

	class Window
	{
	public:
		virtual ~Window() = default;
	public:
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void* GetHandle() const = 0;
	public:
		virtual void OnUpdate() = 0;
		virtual bool ShouldClose() = 0;
	public:
		static Scope<Window> Create(const WindowProperties& p_Properties = WindowProperties());
	};
}
 
#endif // __VIOLET_CORE_WINDOW_H_INCLUDED__