#include "../Renderer/Renderer2D.h"

namespace Violet
{
	Scope<RendererAPI> Renderer2D::s_RendererAPI = RendererAPI::Create();

	void Renderer2D::PreInit(Ref<Window> p_Window)
	{
		s_RendererAPI->PreInit(p_Window);
	}

	void Renderer2D::Init()
	{
		s_RendererAPI->Init();
	}
}