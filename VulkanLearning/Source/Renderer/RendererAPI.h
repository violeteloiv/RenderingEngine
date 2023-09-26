#ifndef __VIOLET_RENDERER_RENDERER_API_H_INCLUDED__
#define __VIOLET_RENDERER_RENDERER_API_H_INCLUDED__

#include "../Core/Pointer.h"
#include "../Core/Window.h"

namespace Violet
{
	class RendererAPI
	{
	public:
		enum class API {
			Unspecified = 0,
			Vulkan = 1,
			OpenGL = 2, // TODO: Implement OpenGL Support
		};
	public:
		virtual ~RendererAPI() = default;
	public:
		virtual void Init() = 0;
	public:
		static API GetAPI() { return s_API; }
		static Scope<RendererAPI> Create();
	private:
		static API s_API;
	};
}

#endif // __VIOLET_RENDERER_RENDERER_API_H_INCLUDED__