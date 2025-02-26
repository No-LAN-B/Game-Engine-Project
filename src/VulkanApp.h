#ifndef VULKAN_APP_H
#define VULKAN_APP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// GLFW will include its own definitions and automatically load the Vulkan header with it

#include <iostream> // Debugging Errors
#include <stdexcept> // Debugging Errors
#include <cstdlib> // Exit Success and Exit Failure
#include <vector>
#include <cstring>
#include <optional> // wrapper that contains no value until you assign something to it. At any point you can query if it contains a value or not by calling its has_value() member function. 


class VulkanApp {
    // any value of uint32_t could in theory be a valid queue family index including 0. 
    // Luckily C++17 introduced a data structure to distinguish between the case of a value existing or not:
    // std::optional (#include <optional> in header file)
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;

        // To make this more convenient, I added a generic check to the struct itself:
        bool isComplete() {
            return graphicsFamily.has_value();
        }
    };
public:
    VulkanApp();
    ~VulkanApp();

    void run();

private:
    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger
    );

    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator
    );

    void initWindow();
    void initVulkan();
    void pickPhysicalDevice();
    void mainLoop();
    void cleanup();
    void createInstance();
    bool checkValidationLayerSupport();
    void createLogicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();
    std::vector<const char*> getRequiredExtensions();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);


    

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    VkQueue graphicsQueue; 
    VkDevice device;
    GLFWwindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
};

#endif // VULKAN_APP_H
