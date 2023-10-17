#include <VioletRenderer.h>

class TestApplication : public Violet::Application
{
public:
	TestApplication()
	{
		Violet::Renderer2D::Init();
	}

	~TestApplication() = default;
};

Violet::Ref<Violet::Application> Violet::CreateApplication()
{
	return Violet::CreateRef<TestApplication>();
}