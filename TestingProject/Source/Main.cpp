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

Violet::Ref<Violet::Application> Violet::CreateApplication()
{
	return Violet::CreateRef<Testing>();
}