#pragma once
#include "Device.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Pipeline.h"

class Renderer {
public:
    Renderer(Device& device, SwapChain& swapChain, RenderPass& renderPass, Pipeline& pipeline);
    ~Renderer();

    void createCommandPool(); // needs fix
    void createCommandBuffers();
    void recordCommandBuffer(uint32_t imageIndex);
    VkCommandBuffer getCurrentCommandBuffer() const;

private:
    Device& device;
    SwapChain& swapChain;
    RenderPass& renderPass;
    Pipeline& pipeline;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
};
