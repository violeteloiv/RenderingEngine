#include <VioletRenderer.h>

class TestApplication : public Violet::Application
{
public:
	typedef std::pair<int, int> StartEnd;

	TestApplication()
	{
		Violet::Renderer2D::Init();

		//std::vector<std::string> lines = Violet::Utils::SplitString(source, '\n');

		//std::vector<StartEnd> startEndPairs = GetPairs(lines);
		//for (auto pair : startEndPairs)
		//{
			
		//}
	}

	std::string GetChunk(StartEnd p_Pair)
	{
		
	}

	~TestApplication() = default;
};

Violet::Ref<Violet::Application> Violet::CreateApplication()
{
	return Violet::CreateRef<TestApplication>();
}