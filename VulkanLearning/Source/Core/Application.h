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
		void InitializeWindow();
		void InitializeVulkan();
		void MainLoop();
		void CleanUp();
		void DrawFrame();
	private:
		void CreateInstance();
		void CreateLogicalDevice();
		void PickPhysicalDevice();
		int RateDeviceSuitability(VkPhysicalDevice p_Device);
	private:
		void CreateSyncObjects();
	private:
		bool CheckValidationLayerSupport();
		std::vector<const char*> GetRequiredExtensions();
		void SetupDebugMessenger();
	private:
		Window* m_Window;
		VkInstance m_Instance;
		VkPhysicalDevice m_PhysicalDevice;
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