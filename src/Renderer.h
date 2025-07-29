#pragma once

#include "Device.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Pipeline.h"

#include <vulkan/vulkan.h>
#include <vector>

class Renderer {
public:
    void init(Device& device_, SwapChain& swapChain_, RenderPass& renderPass_, Pipeline& pipeline_);
    void cleanup();

    void createSyncObjects();
    void createCommandPool();
    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();

    // Declaration of getter
    VkCommandBuffer getCurrentCommandBuffer() const;

private:
    Device* device = nullptr;
    SwapChain* swapChain = nullptr;
    RenderPass* renderPass = nullptr;
    Pipeline* pipeline = nullptr;

    VkCommandPool                   commandPool;
    std::vector<VkCommandBuffer>    commandBuffers;
    
    uint32_t currentImageIndex = 0;
    uint32_t currentFrame = 0;
    const int MAX_FRAMES_IN_FLIGHT = 2;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
};
