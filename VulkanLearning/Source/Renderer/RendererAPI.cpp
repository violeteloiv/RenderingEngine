#include "../Renderer/RendererAPI.h"

#include <stdexcept>

#include "../Platform/Vulkan/VulkanRendererAPI.h"

namespace Violet
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
			case RendererAPI::API::Unspecified: throw std::runtime_error("Please Specify A Rendering API To Use.");
			case RendererAPI::API::Vulkan: return CreateScope<VulkanRendererAPI>();
			case RendererAPI::API::OpenGL: throw std::runtime_error("OpenGL API Is Not Currently Implemented"); // TODO: Implement OpenGL Renderer API.
		}

		throw std::runtime_error("Unknown Renderer API");
	}
}