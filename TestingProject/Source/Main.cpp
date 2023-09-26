#include <VioletRenderer.h>

class Testing : public Violet::Application
{
public:
	Testing()
	{
		Violet::Scope<Violet::RendererAPI> api = Violet::RendererAPI::Create();
		api->Init();
	}

	~Testing() = default;
};

Violet::Application* Violet::CreateApplication()
{
	return new Testing();
}