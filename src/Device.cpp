#include "Device.h"
#include "Utils.h"         // for getRequiredExtensions
#include <stdexcept>
#include <vector>
#include <set>
#include <cstring>
#include "SwapChain.h"   // for SwapChainSupportDetails

void Device::init(GLFWwindow* window, DebugUtils& debugUtils) {
    this->window = window;

#ifdef NDEBUG
    enableValidation = false;
#endif
    debugUtils.setupValidationLayers();
    createInstance("Modor Engine", debugUtils);
    debugUtils.setupDebugMessenger(_instance);
    pickPhysicalDevice();
    createLogicalDevice();
}

void Device::cleanup() {
    vkDestroyDevice(_device, nullptr);
    vkDestroyInstance(_instance, nullptr);
}

VkInstance Device::instance() const { return _instance; }
VkPhysicalDevice Device::physicalDevice() const { return _physical; }
VkDevice Device::device() const { return _device; }
VkQueue Device::graphicsQueue() const { return _graphicsQ; }
VkQueue Device::presentQueue() const { return _presentQ; }

void Device::createInstance(const char* appName, DebugUtils& debugUtils) {
    if (enableValidation && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available");
    }

    VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Modor Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    auto extensions = getRequiredExtensions(enableValidation);
    VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (enableValidation) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }

    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create Vulkan instance");
    }
}

void Device::pickPhysicalDevice() {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(_instance, &count, nullptr);
    if (count == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support");
    }

    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(_instance, &count, devices.data());

    for (const auto& dev : devices) {
        if (isDeviceSuitable(dev)) {
            _physical = dev;
            break;
        }
    }

    if (_physical == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU");
    }
}

void Device::createLogicalDevice() {
    // Find queue families
    auto indices = findQueueFamilies(_physical);

    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    float priority = 1.0f;
    std::set<uint32_t> uniqueFamilies = {
        indices.graphicsFamily.value(),
        indices.presentFamily.value()
    };
    for (uint32_t fam : uniqueFamilies) {
        VkDeviceQueueCreateInfo qi{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        qi.queueFamilyIndex = fam;
        qi.queueCount = 1;
        qi.pQueuePriorities = &priority;
        queueInfos.push_back(qi);
    }

    VkPhysicalDeviceFeatures features{};
    VkDeviceCreateInfo ci{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    ci.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
    ci.pQueueCreateInfos = queueInfos.data();
    ci.pEnabledFeatures = &features;
    ci.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    ci.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidation) {
        ci.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        ci.ppEnabledLayerNames = validationLayers.data();
    }

    if (vkCreateDevice(_physical, &ci, nullptr, &_device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device");
    }

    vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicsQ);
    vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentQ);
}

// Check if all requested validation layers are available
bool Device::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> available(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, available.data());

    for (const char* layerName : validationLayers) {
        bool found = false;
        for (const auto& prop : available) {
            if (std::strcmp(prop.layerName, layerName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

// Find queue families (graphics and present)
Device::QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice dev) {
    QueueFamilyIndices indices;
    
        uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, nullptr);
    
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, queueFamilies.data());
    
            // Original: find both graphics and present support, using surface
        uint32_t i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
            
        }
        if (indices.isComplete()) {
            break;
        }
        i++;
    }
    return indices;
}

// Check for required device extensions (e.g., swapchain)
bool Device::checkDeviceExtensionSupport(VkPhysicalDevice dev) {
    uint32_t extCount;
    vkEnumerateDeviceExtensionProperties(dev, nullptr, &extCount, nullptr);

    std::vector<VkExtensionProperties> available(extCount);
    vkEnumerateDeviceExtensionProperties(dev, nullptr, &extCount, available.data());

    std::set<std::string> required(deviceExtensions.begin(), deviceExtensions.end());
    for (const auto& ext : available) {
        required.erase(ext.extensionName);
    }
    return required.empty();
}

// Determine if a device is suitable: has necessary queue families and extensions
bool Device::isDeviceSuitable(VkPhysicalDevice device) {
    // use queue lookup function :)
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    // utilize this function to verify that swap chain support is adequate.
    bool swapChainAdequate = false;
    if (extensionsSupported) {
        // sufficient if there is at least one supported image format and one supported presentation mode given the window surface we have
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    // It is important that we only try to query for swap chain support after verifying that the extension is available.
    // The last line of the function changes to:
    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(_physical, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

// Insert this definition after those methods:
SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice physDev) const {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDev, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physDev, surface, &formatCount, nullptr);
    if (formatCount > 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            physDev, surface, &formatCount, details.formats.data());
    }

    uint32_t presentCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physDev, surface, &presentCount, nullptr);
    if (presentCount > 0) {
        details.presentModes.resize(presentCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physDev, surface, &presentCount, details.presentModes.data());
    }

    return details;
}

