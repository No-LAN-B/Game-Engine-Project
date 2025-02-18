#ifndef VULKAN_APP_H
#define VULKAN_APP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// GLFW will include its own definitions and automatically load the Vulkan header with it

#include <iostream> // Debugging Errors
#include <stdexcept> // Debugging Errors
#include <cstdlib> // Exit Success and Exit Failure

class VulkanApp {
public:
    VulkanApp();
    ~VulkanApp();

    void run();

private:
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();
    void createInstance();

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    GLFWwindow* window;
    VkInstance instance;
};

#endif // VULKAN_APP_H
