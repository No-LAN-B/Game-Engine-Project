// src/VulkanApp.h
#ifndef VULKAN_APP_H
#define VULKAN_APP_H

// for native window creation (although we’re using GLFW here)
// #define GLFW_EXPOSE_NATIVE_WIN32
// #include <GLFW/glfw3native.h>
// #define VK_USE_PLATFORM_WIN32_KHR

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// High-level application class: orchestrates window, Vulkan setup, main loop, cleanup.
#include "Device.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Pipeline.h"
#include "DebugUtils.h"

class VulkanApp {
public:
    VulkanApp();
    ~VulkanApp();

    // Runs the application:
    //   - initWindow()
    //   - initVulkan()
    //   - mainLoop()
    //   - cleanup()
    void run();

private:
    // Create and configure the GLFW window.
    void initWindow();

    // Create Vulkan instance, debug messenger, surface, device, swapchain, pipeline, etc.
    void initVulkan();

    // Poll events until the user closes the window.
    void mainLoop();

    // Destroys pipelines, swapchain, device, debug messenger, surface, instance, window.
    void cleanup();

    // Fixed window dimensions
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    GLFWwindow* window = nullptr;

    // Subsystem managers
    Device     device;      // instance, physical & logical device, queues
    SwapChain  swapChain;   // swapchain creation, image views
    RenderPass renderPass;
    Pipeline   pipeline;    // graphics pipeline + dynamic state
    DebugUtils debugUtils;  // validation layers & debug messenger
};

#endif // VULKAN_APP_H