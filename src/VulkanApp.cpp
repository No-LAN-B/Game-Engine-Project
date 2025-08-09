// src/VulkanApp.cpp
#include "VulkanApp.h"
#include "Renderer.h"
#include <stdexcept> // for runtime_error

VulkanApp::VulkanApp() {
    // nothing special
}

VulkanApp::~VulkanApp() {
    // cleanup() handles destruction
}

void VulkanApp::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void VulkanApp::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, VulkanApp::framebufferResizeCallback);
}
void VulkanApp::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = static_cast<VulkanApp*>(glfwGetWindowUserPointer(window));
    app->setFramebufferResized(true);
}

void VulkanApp::initVulkan() {
    debugUtils.setupValidationLayers();
    device.init(window, debugUtils);
    swapChain.init(device, window); // creates swapchain + image views

    renderPass.init(device, swapChain); // creates VkRenderPass
    pipeline.init(device, swapChain, renderPass); // creates graphics pipeline

    //build the framebuffers now that renderPass is valid
    swapChain.createFramebuffers(device, renderPass);

    // now the renderer can size its command buffers to match those framebuffers
    renderer.init(device, swapChain, renderPass, pipeline);
}

void VulkanApp::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        renderer.drawFrame();
    }
    // Wait for GPU before destroying resources
    vkDeviceWaitIdle(device.device());
}

void VulkanApp::cleanup() {
    renderer.cleanup();
    pipeline.cleanup();
    renderPass.cleanup(device);

    // destroy framebuffers before tearing down the swapchain
    swapChain.cleanupFramebuffers(device);
    swapChain.cleanup();

    debugUtils.cleanup(device.instance());

    device.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();
}
