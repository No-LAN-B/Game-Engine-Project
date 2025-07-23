// src/DebugUtils.cpp
#include "DebugUtils.h"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

DebugUtils::DebugUtils() {
#ifdef NDEBUG
    enableValidation = false;
#endif
}

DebugUtils::~DebugUtils() = default;

void DebugUtils::setupValidationLayers() {
    if (enableValidation && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }
}

void DebugUtils::setupDebugMessenger(VkInstance instance) {
    if (!enableValidation) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void DebugUtils::cleanup(VkInstance instance) {
    if (enableValidation) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
}

bool DebugUtils::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool found = false;
        for (const auto& prop : availableLayers) {
            if (std::strcmp(prop.layerName, layerName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

void DebugUtils::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& ci) {
    ci = {};
    ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    ci.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    ci.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    ci.pfnUserCallback = debugCallback;
}

VkResult DebugUtils::CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DebugUtils::DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func) {
        func(instance, debugMessenger, pAllocator);
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtils::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}
