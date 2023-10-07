#include <VioletRenderer.h>

class Testing : public Violet::Application
{
public:
	Testing()
	{
		Violet::Renderer2D::Init();
	}

	~Testing() = default;
};

Violet::Application* Violet::CreateApplication()
{
	return new Testing();
}