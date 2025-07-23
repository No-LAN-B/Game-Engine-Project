// src/Pipeline.cpp
#include "Pipeline.h"
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
    // -- Load shader bytecode
    auto vertShaderCode = readFile("shaders/vert.spv");
    auto fragShaderCode = readFile("shaders/frag.spv");

    // -- Create shader modules
    VkShaderModule vertModule = createShaderModule(vertShaderCode);
    VkShaderModule fragModule = createShaderModule(fragShaderCode);

    // -- Shader stage infos
    VkPipelineShaderStageCreateInfo vertStageCI{};
    vertStageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStageCI.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStageCI.module = vertModule;
    vertStageCI.pName = "main";

    VkPipelineShaderStageCreateInfo fragStageCI{};
    fragStageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStageCI.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStageCI.module = fragModule;
    fragStageCI.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertStageCI, fragStageCI };

    // -- FIXED-FUNCTION SETUP (vertex input, input assembly, viewport, rasterizer, multisample, color blend)
    // TODO: Fill in VkPipelineVertexInputStateCreateInfo, VkPipelineInputAssemblyStateCreateInfo, VkViewport,
    //       VkPipelineViewportStateCreateInfo, VkPipelineRasterizationStateCreateInfo, VkPipelineMultisampleStateCreateInfo,
    //       VkPipelineColorBlendAttachmentState + VkPipelineColorBlendStateCreateInfo.

    // -- Dynamic state (viewport + scissor)
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynStateCI{};
    dynStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynStateCI.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynStateCI.pDynamicStates = dynamicStates.data();

    // -- Pipeline layout (no descriptor sets or push constants yet)
    VkPipelineLayoutCreateInfo layoutCI{};
    layoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutCI.setLayoutCount = 0;
    layoutCI.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(device->device(), &layoutCI, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    // -- Graphics pipeline creation
    VkGraphicsPipelineCreateInfo pipelineCI{};
    pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCI.stageCount = 2;
    pipelineCI.pStages = shaderStages;
    // TODO: wire up your fixed-function structs here:
    // pipelineCI.pVertexInputState   = &vertexInputCI;
    // pipelineCI.pInputAssemblyState = &inputAssemblyCI;
    // VkPipelineViewportStateCreateInfo viewportStateCI{...};
    // pipelineCI.pViewportState      = &viewportStateCI;
    // VkPipelineRasterizationStateCreateInfo rasterizerCI{...};
    // pipelineCI.pRasterizationState = &rasterizerCI;
    // pipelineCI.pMultisampleState   = &multisampleCI;
    // VkPipelineColorBlendStateCreateInfo colorBlendCI{...};
    // pipelineCI.pColorBlendState    = &colorBlendCI;

    pipelineCI.pDynamicState = &dynStateCI;
    pipelineCI.layout = pipelineLayout;
    //pipelineCI.renderPass = /* TODO: your VkRenderPass handle */;
    pipelineCI.subpass = 0;

    if (vkCreateGraphicsPipelines(device->device(),
        VK_NULL_HANDLE,
        1,
        &pipelineCI,
        nullptr,
        &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    // -- Clean up shader modules
    vkDestroyShaderModule(device->device(), fragModule, nullptr);
    vkDestroyShaderModule(device->device(), vertModule, nullptr);
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
