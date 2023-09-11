#include "../Renderer/GraphicsPipeline.h"

#include <stdexcept>

#include "../Core/Utils.h"

// TODO: Reorganize Graphics Pipeline Into Multiple Submodules:
//       - Shader Submodule
//       - Dynamic States Submodule
//       - Vertex Buffer/Array Submodule
//       - Element Buffer Submodule
//       - Viewport Submodule
//       - Rasterizer Submodule
//       - Uniform Buffer Submodule
namespace Renderer
{
	GraphicsPipeline::GraphicsPipeline(VkDevice p_Device, SwapChain p_SwapChain, RenderPass p_RenderPass)
		: m_Device(p_Device)
	{
		// Find the shader pipeline stages.
		FindShaderPipelineStages();

		// If we want to have some dynamic attributes of the pipeline,
		// such as viewport size, line width, or blend constants, we
		// have to be explicit about such.

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
		};
	
		// Create the info for dynamic states.
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		// We also need to describe the data being passed into the vertex shader.
		// It does this using bindings, or the spacing between data and whether
		// the data is pre-vertex or per-instance. Also attribute descriptions,
		// which is the type of attribute, which binding to load them from, and
		// which offset.

		// We are going to start by specifying no data.
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		// We also need to describe what kind of geometry will be drawn and
		// if primitve restart should be enabled. This is the topology member
		// and is specified by:
		// VK_PRIMITIVE_TOPOLOGY_POINT_LIST: Points from vertices.
		// VK_PRIMITIVE_TOPOLOGY_LINE_LIST: Line from every 2 vertices without
		// reuse.
		// VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: The end vertex of every line is
		// used as the start for the next line.
		// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: Triangle from every three
		// vertices without reuse.
		// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: The second and third vertex
		// of every triangle is used as the first two vertices for the next
		// triangle.

		// Normally, the vertices are loaded from the vertex buffer
		// by index in sequential order, but with an element buffer one can
		// specify the order to draw them. This allows us to use optimizations.

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// A viewport is a region of the framebuffer that the output will be rendered to.
		// By default we want it to be (0, 0) to (width, height).

		VkExtent2D extent = p_SwapChain.GetSpecifiedDetails().Extent;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)extent.width;
		viewport.height = (float)extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		// A scissor rectangle specifies the range to draw to. It can differ from the viewport
		// in the sense that if it's smaller, it will cut off part of the image.

		// We want the scissor to match the viewport.
		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;

		// Actually create the viewport state info.
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		// The rasterizer takes the geometry and turns it into fragments to be colored by
		// the fragment shader. It performs depth testing, face culling, and the scissor test.

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		// VK_FALSE means that fragments further and closer than the planes are discarded.
		// making this true requires enabling a GPU feature.
		rasterizer.depthClampEnable = VK_FALSE;
		// Setting to true means that geometry never gets through the rasterizer.
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		// Determines how fragments are generated for geometry. This fills the area
		// of the polygon. LINE would draw the edges, and POINT would draw the vertices
		// as points.
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		// Line width. Thicker than 1.0f requires enabled GPU feature.
		rasterizer.lineWidth = 1.0f;
		// cull mode determines the type of face culling to use. Front faces tells you
		// the way to determine which are front faces.
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		// A rasterizer can alter the depth values for shadow mapping, but won't be
		// used.
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		// Multi-Sampling is one of the ways to perform anti-aliasing. Enabling requires
		// a GPU feature.
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		// TODO: Depth & Stencil Testing.

		// After a fragment shader returns a color, it needs to be combined with the
		// color that it already is in the framebuffer. We need two structs to handle
		// color blending.

		// We implement alpha blending.
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Blending operation.
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Blending operation.

		// The other structure references the array of structures of the framebuffers
		// and allows you to set blend constant that you can use as blend factors.
		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		// Set color blending constants.
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		// For uniform buffers we have to set the layout on the pipeline. Since we
		// don't have any uniform buffers yet we keep most 0 and nullptr.
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		// Create the layout
		if (vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed To Create Pipeline Layout!");

		// Now with all of this information, we create the pipeline.
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		// Reference the shader stages.
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = m_ShaderStageCreateInfos.data();
		// Describe the fixed function state.
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_PipelineLayout;
		// Render pass information.
		pipelineInfo.renderPass = p_RenderPass.GetHandle();
		pipelineInfo.subpass = 0;
		// Vulkan allows us to create a new pipeline deriving from an already created one.
		// We will keep this blank for now.
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		// Finally create the pipeline object.
		if (vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed To Create Graphics Pipeline!");
	}

	void GraphicsPipeline::Shutdown()
	{
		// Destroy pipeline.
		vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);

		// Destroy pipeline layout.
		vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);

		// Destroy shader modules.
		vkDestroyShaderModule(m_Device, m_FragShaderCode, nullptr);
		vkDestroyShaderModule(m_Device, m_VertShaderCode, nullptr);
	}

	VkPipeline GraphicsPipeline::GetHandle()
	{
		return m_GraphicsPipeline;
	}

	void GraphicsPipeline::FindShaderPipelineStages()
	{
		auto vertShaderCode = Core::Utils::ReadFile("Shaders/vert.spv");
		auto fragShaderCode = Core::Utils::ReadFile("Shaders/frag.spv");

		m_VertShaderCode = CreateShaderModule(vertShaderCode);
		m_FragShaderCode = CreateShaderModule(fragShaderCode);

		// Before we can use the shaders we have to assign them to a specific
		// pipeline stage.

		// Vertex Shader
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; // Assign to vertex stage.
		vertShaderStageInfo.module = m_VertShaderCode; // A link to the shader module.
		vertShaderStageInfo.pName = "main"; // The entrypoint.

		// Fragment Shader
		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT; // Assign to fragment stage.
		fragShaderStageInfo.module = m_FragShaderCode; // A link to the shader module.
		fragShaderStageInfo.pName = "main"; // The entrypoint.

		m_ShaderStageCreateInfos = { vertShaderStageInfo, fragShaderStageInfo };
	}

	VkShaderModule GraphicsPipeline::CreateShaderModule(const std::vector<char>& p_Code)
	{
		// To create the shader module, all we have to do is give a pointer to the byte code.
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = p_Code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(p_Code.data());

		// We now easily make the shader module.
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed To Create Shader Module!");

		return shaderModule;
	}
}