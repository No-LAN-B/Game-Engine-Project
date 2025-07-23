// src/VulkanApp.cpp
#include "VulkanApp.h"
#include <stdexcept> // for runtime_error

VulkanApp::VulkanApp() {
    // Constructor: nothing special here
}

VulkanApp::~VulkanApp() {
    // Destructor: cleanup() handles all resource destruction
}

void VulkanApp::run() {
    // Runs the application:
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void VulkanApp::initWindow() {
    // Create and configure the GLFW window.
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void VulkanApp::initVulkan() {
    // Create Vulkan instance, debug messenger, surface, device, swapchain, pipeline, etc.
    debugUtils.setupValidationLayers();            // populate & create debug messenger
    device.init(window, debugUtils);         // instance, pick physical & create logical device
    swapChain.init(device, window);          // swapchain creation + image views
    pipeline.init(device, swapChain);        // graphics pipeline (with dynamic state)
}

void VulkanApp::mainLoop() {
    // Polls events until the user closes the window.
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        // drawFrame(); // you can hook in a Renderer here later
    }
}

void VulkanApp::cleanup() {
    // Destroys pipelines, swapchain, device, debug messenger, surface, instance, window.
    pipeline.cleanup();
    swapChain.cleanup();
    device.cleanup();
    debugUtils.cleanup(device.instance());

    glfwDestroyWindow(window);
    glfwTerminate();
}
