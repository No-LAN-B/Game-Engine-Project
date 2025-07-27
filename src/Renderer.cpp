#include "Renderer.h"
#include "Device.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Pipeline.h"

#include <stdexcept>
#include <vector>
#include <array>

void Renderer::init(Device& device_, SwapChain& swapChain_, RenderPass& renderPass_, Pipeline& pipeline_) {
    // assign the pointers
    device = &device_;
    swapChain = &swapChain_;
    renderPass = &renderPass_;
    pipeline = &pipeline_;

    createCommandPool();
    createCommandBuffer();
}

void Renderer::cleanup(){
    vkDestroyCommandPool(device->device(), commandPool, nullptr);
}

void Renderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->get();
    renderPassInfo.framebuffer = swapChain->getFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChain->getExtent();

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->get());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChain->getExtent().width;
    viewport.height = (float)swapChain->getExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChain->getExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void Renderer::createCommandPool() {

    Device::QueueFamilyIndices queueFamilyIndices = device->findQueueFamilies(device->physicalDevice());

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(device->device(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}
void Renderer::createCommandBuffer() {
    // 1) size your storage to match how many you need:
    size_t count = swapChain->getFramebuffers().size();
    commandBuffers.resize(count);

    // 2) fill out the allocator info
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;                         // your pool
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(count);

    // 3) allocate into the array/vector .data()
    if (vkAllocateCommandBuffers(device->device(),
        &allocInfo,
        commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    // 4) (optional) record each buffer
    for (size_t i = 0; i < count; i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // … any flags …

        vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
        // record your draw calls into commandBuffers[i]…
        vkEndCommandBuffer(commandBuffers[i]);
    }
}

    