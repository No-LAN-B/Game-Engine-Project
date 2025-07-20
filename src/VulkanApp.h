#ifndef VULKAN_APP_H
#define VULKAN_APP_H

// for native window creation (although I'll be using glfw instead but its here anyways)
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <GLFW/glfw3native.h>
//#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// GLFW will include its own definitions and automatically load the Vulkan header with it

#include <iostream> // Debugging Errors
#include <stdexcept> // Debugging Errors
#include <cstdlib> // Exit Success and Exit Failure
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cstring>
#include <limits>
#include <optional> // wrapper that contains no value until you assign something to it. At any point you can query if it contains a value or not by calling its has_value() member function. 
#include <set>

// any value of uint32_t could in theory be a valid queue family index including 0. 
    // Luckily C++17 introduced a data structure to distinguish between the case of a value existing or not:
    // std::optional (#include <optional> in header file)
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    // To make this more convenient, I added a generic check to the struct itself:
    bool isComplete() {
        return graphicsFamily.has_value();
    }
};
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
class VulkanApp {   
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
        void createImageViews();
        void createGraphicsPipeline();
        void pickPhysicalDevice();
        void mainLoop();
        void cleanup();
        void createInstance();
        bool checkValidationLayerSupport();
        void createLogicalDevice();
        bool isDeviceSuitable(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void setupDebugMessenger();
        void createSurface();
        std::vector<const char*> getRequiredExtensions();
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    
        // Swap Chain
        void createSwapChain();
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);

        // Window Stuff
        const uint32_t WIDTH = 800;
        const uint32_t HEIGHT = 600;
        const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        VkSurfaceKHR surface;
        VkQueue presentQueue;

        // Vulkan Stuff
        VkQueue graphicsQueue; 
        VkDevice device;
        GLFWwindow* window;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;

        //Vulkan Swap Chain
        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;

        //Vulkan Images
        std::vector<VkImageView> swapChainImageViews;
    };

#endif // VULKAN_APP_H
