#ifndef __VIOLET_RENDERER_H_INCLUDED__
#define __VIOLET_RENDERER_H_INCLUDED__

#include "../Renderer/RendererAPI.h"

namespace Violet
{
	class Renderer2D
	{
	public:
		Renderer2D(Renderer2D& p_Other) = delete;
		void operator=(const Renderer2D&) = delete;
	public:
		static void Init();
	protected:
		static Scope<RendererAPI> s_RendererAPI;
	};
}

#endif // __VIOLET_RENDERER_H_INCLUDED__