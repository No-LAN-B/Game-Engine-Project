// src/VulkanApp.h
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Device.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Pipeline.h"
#include "Renderer.h"
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
    void initWindow();
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    void initVulkan();
    void mainLoop();
    void cleanup();
    void setFramebufferResized(bool resized) {
        renderer.framebufferResized = resized;
    };

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    GLFWwindow* window = nullptr;

    // Subsystem managers
    DebugUtils debugUtils;
    Device     device;
    SwapChain  swapChain;
    RenderPass renderPass;
    Pipeline   pipeline;
    Renderer   renderer;
};

