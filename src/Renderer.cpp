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
    createCommandBuffers();
    createSyncObjects();
}

void Renderer::cleanup() {
    vkDeviceWaitIdle(device->device());

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device->device(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device->device(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device->device(), inFlightFences[i], nullptr);
    }
    if (commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device->device(), commandPool, nullptr);
        commandPool = VK_NULL_HANDLE;
    }
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
void Renderer::createCommandBuffers() {
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
    // 1) Wait on the previous frame’s GPU work
    vkWaitForFences(device->device(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(device->device(), 1, &inFlightFences[currentFrame]);

    // 2) Grab the next swapchain image directly into currentImageIndex
    vkAcquireNextImageKHR(
        device->device(),
        swapChain->getSwapChain(),
        UINT64_MAX,
        imageAvailableSemaphores[currentFrame],
        VK_NULL_HANDLE,
        &currentImageIndex
    );

    // 3) Re-record this frame’s command buffer against the newly acquired image
    vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    recordCommandBuffer(commandBuffers[currentFrame], currentImageIndex);

    // 4) Submit to the graphics queue
    VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(device->graphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // 5) Present, using that same currentImageIndex
    VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { swapChain->getSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &currentImageIndex;

    vkQueuePresentKHR(device->presentQueue(), &presentInfo);

    // 6) Advance to the next frame slot
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}



void Renderer::createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device->device(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device->device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device->device(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }

}

VkCommandBuffer Renderer::getCurrentCommandBuffer() const {
    return commandBuffers[currentImageIndex];
}
    