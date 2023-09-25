#include <VioletRenderer.h>

class Testing : public Violet::Application
{
public:
	Testing() = default;
	~Testing() = default;
};

Violet::Application* Violet::CreateApplication()
{
	return new Testing();
}