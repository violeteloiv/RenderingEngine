#include "../Core/Debug.h"

#include <stdexcept>

#include "../Platform/Vulkan/VulkanDebug.h"
#include "../Renderer/RendererAPI.h"

namespace Violet
{
	Ref<Debug> Debug::Create()
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::Unspecified: throw std::runtime_error("Please Specify A Rendering API To Use.");
			case RendererAPI::API::Vulkan: return CreateRef<VulkanDebug>();
			case RendererAPI::API::OpenGL: throw std::runtime_error("OpenGL API Is Not Currently Implemented"); // TODO: Implement OpenGL Renderer API.
		}

		throw std::runtime_error("Unknown Renderer API");
	}
}