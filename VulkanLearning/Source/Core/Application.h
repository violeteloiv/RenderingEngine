#ifndef __VIOLET_CORE_APPLICATION_H__
#define __VIOLET_CORE_APPLICATION_H__

#include "../Core/Window.h"

#include <vector>

namespace Core
{
	class Application
	{
	public:
		Application();
		~Application();
	public:
		void Run();
	private:
		void InitializeVulkan();
		void CleanUp();
		void DrawFrame();
	private:
		void CreateLogicalDevice();
	private:
		void CreateSyncObjects();
	private:
		Violet::Ref<Violet::Window> m_Window;
		VkDevice m_Device;
	private:
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
	private:
		VkSemaphore m_ImageAvailableSemaphore;
		VkSemaphore m_RenderFinishedSemaphore;
		VkFence m_InFlightFence;
	};
}

#endif // __VIOLET_CORE_APPLICATION_H__