// src/Pipeline.cpp
#include "Pipeline.h"
#include "SwapChain.h"
#include "Device.h"
#include "Utils.h"       // for readFile()
#include <stdexcept>
#include <array>

void Pipeline::init(Device& dev, SwapChain& sc) {
    device = &dev;
    swapChain = &sc;
    createGraphicsPipeline();
}

void Pipeline::cleanup() {
    vkDestroyPipeline(device->device(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device->device(), pipelineLayout, nullptr);
}

void Pipeline::createGraphicsPipeline() {
    // Load shader bytecode
    auto vertShaderCode = readFile("shaders/vert.spv");
    auto fragShaderCode = readFile("shaders/frag.spv");

    // Create shader modules
    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    // Shader stage infos
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    // Instead of querying swapChainExtent directly, do:
    auto extent = swapChain->getExtent();

    // A viewport basically describes the region of the framebuffer that the output will be rendered to. 
    // This will almost always be (0, 0) to (width, height)
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;

    // The actual viewport(s) and scissor rectangle(s) will then later be set up at drawing time.
    // With dynamic state it's even possible to specify different viewports and or scissor rectangles within a single command buffer.
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    // Because we're hard coding the vertex data directly in the vertex shader,
    // we'll fill in this structure to specify that there is no vertex data to load for now.
    // vertex buffer will be implemented soon :D
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    // Bindings: spacing between data and whether the data is per-vertex or per-instance
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
    // Attribute descriptions: type of the attributes passed to the vertex shader, 
    // which binding to load them from and at which offset
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional
    // The pVertexBindingDescriptions and pVertexAttributeDescriptions members point to an array of structs that describe the-
    // aforementioned details for loading vertex data. 
    

    // The rasterizer takes the geometry that is shaped by the vertices from the vertex shader and 
    // turns it into fragments to be colored by the fragment shader.
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // If depthClampEnable is set to VK_TRUE, then fragments that are beyond the near and far planes are clamped to them as opposed to discarding them.
    // This is useful in some special cases like shadow maps. Using this requires enabling a GPU feature.
    rasterizer.depthClampEnable = VK_FALSE;
    // If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes through the rasterizer stage. 
    // This basically disables any output to the framebuffer.
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    // The polygonMode determines how fragments are generated for geometry. The following modes are available:
                // VK_POLYGON_MODE_FILL: fill the area of the polygon with fragments
                // VK_POLYGON_MODE_LINE: polygon edges are drawn as lines
                // VK_POLYGON_MODE_POINT: polygon vertices are drawn as points
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    // The lineWidth member is straightforward, it describes the thickness of lines in terms of number of fragments.
    // NOTE - any line thicker than 1.0f requires you to enable the wideLines GPU feature.
    rasterizer.lineWidth = 1.0f;
    // cullMode variable determines the type of face culling to use. cull the front faces, cull the back faces or both.
    // The frontFace variable specifies the vertex order for faces to be considered front-facing and can be clockwise or counterclockwise.
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    // The rasterizer can alter the depth values by adding a constant value or biasing them based on a fragment's slope. for now not using it
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
    

    // Dynamic state (viewport + scissor)
    std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
    };
    // only need to specify their count at pipeline creation time:
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();


    VkPipelineLayout pipelineLayout;
    // Pipeline layout (no descriptor sets or push constants yet)
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional
    // COOL THING !
    // Normally, the vertices are loaded from the vertex buffer by index in sequential order,
    // but with an element buffer you can specify the indices to use yourself. 
    // This allows you to perform optimizations like reusing vertices. If you set the primitiveRestartEnable member to VK_TRUE, 
    // then it's possible to break up lines and triangles in the _STRIP topology modes by using a special index of 0xFFFF or 0xFFFFFFFF
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;


    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    // Clean up shader modules
    vkDestroyShaderModule(device->device(), fragShaderModule, nullptr);
    vkDestroyShaderModule(device->device(), vertShaderModule, nullptr);
}

VkShaderModule Pipeline::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = code.size();
    ci.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule module;
    if (vkCreateShaderModule(device->device(), &ci, nullptr, &module) != VK_SUCCESS)
        throw std::runtime_error("failed to create shader module!");

    return module;
}
