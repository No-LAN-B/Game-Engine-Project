// src/Pipeline.h
#pragma once

#include <vulkan/vulkan.h>   // VkPipeline, VkPipelineLayout, VkRenderPass, VK_NULL_HANDLE
#include <vector>            // std::vector<char>

#include "Device.h"
#include "SwapChain.h"
#include "RenderPass.h"      // for the RenderPass wrapper

/// Encapsulates creation & cleanup of the Vulkan graphics pipeline.
class Pipeline {
public:
    /// Initialize the pipeline.
    ///  • dev provides vkDevice via dev.device()  
    ///  • sc provides swapchain extent via sc.getExtent()  
    ///  • rp provides the VkRenderPass via rp.get()
    void init(Device& dev, SwapChain& sc, RenderPass& rp);

    /// Destroy the pipeline object and its layout (in that order).
    void cleanup();

    /// Raw VkPipeline for vkCmdBindPipeline(…).
    VkPipeline       get()    const { return graphicsPipeline; }

    /// VkPipelineLayout for descriptor sets / push constants.
    VkPipelineLayout layout() const { return pipelineLayout; }

private:
    /// Builds shader stages, fixed-function state, dynamic state, etc.
    void createGraphicsPipeline();

    /// Helper to wrap vkCreateShaderModule().
    VkShaderModule createShaderModule(const std::vector<char>& code);

    //------------------------------------------------------------------------
    // Set in init():
    //------------------------------------------------------------------------
    Device* device = nullptr;         // wrapper for VkDevice
    SwapChain* swapChain = nullptr;         // wrapper for extent/format
    VkRenderPass vkRenderPassHandle = VK_NULL_HANDLE;  // raw handle from RenderPass

    //------------------------------------------------------------------------
    // Owned & destroyed here:
    //------------------------------------------------------------------------
    VkPipeline       graphicsPipeline = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
};
