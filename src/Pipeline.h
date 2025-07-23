// src/Pipeline.h
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Device.h"
#include "SwapChain.h"

// Encapsulates creation and cleanup of the graphics pipeline.
class Pipeline {
public:
    // Set up the graphics pipeline (shaders + fixed-function stages + dynamic state)
    void init(Device& device, SwapChain& swapChain);

    // Destroy pipeline and its layout
    void cleanup();

    // Accessors for drawing
    VkPipeline       get()    const { return graphicsPipeline; }
    VkPipelineLayout layout() const { return pipelineLayout; }

private:
    void createGraphicsPipeline();
    VkShaderModule createShaderModule(const std::vector<char>& code);

    Device* device = nullptr;
    SwapChain* swapChain = nullptr;

    VkPipeline       graphicsPipeline = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
};
