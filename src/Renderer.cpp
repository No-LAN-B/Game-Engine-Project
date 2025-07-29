#include "Renderer.h"
#include "Device.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Pipeline.h"
#include "VulkanApp.h"

#include <stdexcept>
#include <vector>
#include <array>
#include <iostream>

void Renderer::init(Device& device_, SwapChain& swapChain_, RenderPass& renderPass_, Pipeline& pipeline_) {
    // assign the pointers
    device = &device_;
    swapChain = &swapChain_;
    renderPass = &renderPass_;
    pipeline = &pipeline_;

    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
}

void Renderer::cleanup(){
    vkDeviceWaitIdle(device->device());

    vkDestroySemaphore(device->device(), renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(device->device(), imageAvailableSemaphore, nullptr);
    vkDestroyFence(device->device(), inFlightFence, nullptr);
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

void Renderer::drawFrame() {
    // 1) Wait for the previous frame’s fence
    vkWaitForFences(device->device(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device->device(), 1, &inFlightFence);

    // 2) Acquire next image from the swapchain
    uint32_t imageIndex;
    vkAcquireNextImageKHR(
        device->device(),
        swapChain->getSwapChain(),          // <-- make sure you have VkSwapchainKHR getSwapChain() const;
        UINT64_MAX,
        imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &imageIndex
    );

    // remember which buffer we’ll use this frame
    currentImageIndex = imageIndex;

    // grab the command buffer for this image
    VkCommandBuffer cmdBuf = commandBuffers[currentImageIndex];

    // 3) Record & submit it
    vkResetCommandBuffer(cmdBuf, 0);
    recordCommandBuffer(cmdBuf, currentImageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuf;
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(device->graphicsQueue(), 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // 4) Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR sc[] = { swapChain->getSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = sc;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(device->presentQueue(), &presentInfo);
}


void Renderer::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    //build fence in signalled state so vkwait doesnt hold indefinitely for fences that wont come
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(device->device(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device->device(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(device->device(), &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }

}

VkCommandBuffer Renderer::getCurrentCommandBuffer() const {
    return commandBuffers[currentImageIndex];
}
    