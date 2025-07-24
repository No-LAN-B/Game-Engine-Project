// src/RenderPass.h
#pragma once

#include <vulkan/vulkan.h> 

// Forward declarations
class Device;
class SwapChain;

class RenderPass {
public:
    /// Builds the VkRenderPass using the given device and swapchain settings.
    void init(Device& device, SwapChain& swapChain);

    /// Destroys the VkRenderPass.
    void cleanup(Device& device);

    /// Accessor for the render-pass handle.
    VkRenderPass get() const { return renderPass; }

private:
    /// Actually fills out the VkRenderPassCreateInfo and calls vkCreateRenderPass.
    void createRenderPass(Device& device, SwapChain& swapChain);

    VkRenderPass renderPass = VK_NULL_HANDLE;
};