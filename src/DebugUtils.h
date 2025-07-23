// src/DebugUtils.h
#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class DebugUtils {
public:
    DebugUtils();
    ~DebugUtils();

    // Call before vkCreateInstance to validate layer support.
    void setupValidationLayers();

    // After VkInstance is created, set up the debug messenger.
    void setupDebugMessenger(VkInstance instance);

    // Destroy the debug messenger (call before vkDestroyInstance).
    void cleanup(VkInstance instance);

private:
    // Populate the VkDebugUtilsMessengerCreateInfoEXT struct.
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    // Check whether all requested validation layers are available.
    bool checkValidationLayerSupport();

    // Helper to load the extension function for creating the messenger.
    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger);

    // Helper to load the extension function for destroying the messenger.
    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator);

    // Callback invoked by the validation layers.
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    bool enableValidation = true;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
};
