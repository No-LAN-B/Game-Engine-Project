#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <optional>
#include <set>
#include "DebugUtils.h"
#include "SwapChain.h"   // for SwapChainSupportDetails

// Manages instance, physical device selection, logical device, and queues.
class Device {
public:
    // Initialize Vulkan instance, debug messenger, pick & create devices.
    void init(GLFWwindow* window, DebugUtils& debugUtils);

    // Cleanup Vulkan objects.
    void cleanup();

    // Accessors:
    // Also noticed some people spacing their declarations in headers like this and it makes it alot easier to read I cant lie
    VkInstance        instance()       const;
    VkPhysicalDevice  physicalDevice() const;
    VkDevice          device()         const;
    VkQueue           graphicsQueue()  const;
    VkQueue           presentQueue()   const;

    // Helpers for finding queue families:
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
    };
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physDev) const;

private:
    void createInstance(const char* appName, DebugUtils& debugUtils);
    void pickPhysicalDevice();
    void createLogicalDevice();

    bool checkValidationLayerSupport();
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    
   
    // Member data:
    GLFWwindow* window = nullptr;
    VkInstance _instance = VK_NULL_HANDLE;
    VkPhysicalDevice _physical = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VkQueue _graphicsQ = VK_NULL_HANDLE;
    VkQueue _presentQ = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    // Extensions & validation:
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    bool enableValidation = true;  // set in init() based on NDEBUG
};
