// src/SwapChain.h
#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>

// Forward declare Device wrapper to break the include cycle.
class Device;

// Holds the query results for swapchain support.
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

class SwapChain {
public:
    // Initialize window surface, swap chain, and image views.
    void init(Device& device, GLFWwindow* window);

    // Destroy image views, swap chain, and surface.
    void cleanup();

    // Accessors for rendering code.
    VkFormat                        getImageFormat() const { return swapChainImageFormat; }
    VkExtent2D                      getExtent()      const { return swapChainExtent; }
    const std::vector<VkImageView>& getImageViews()  const { return imageViews; }

private:
    // Internal setup steps.
    void createSwapChain();
    void createImageViews();

    // Helpers for querying swapchain support.
    SwapChainSupportDetails  querySwapChainSupport(VkPhysicalDevice physDev, VkSurfaceKHR surface);
    VkSurfaceFormatKHR       chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR         chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D               chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    // State
    Device* device = nullptr;
    GLFWwindow* window = nullptr;
    VkSwapchainKHR          swapChain = VK_NULL_HANDLE;
    std::vector<VkImage>    images;
    std::vector<VkImageView> imageViews;
    VkFormat                swapChainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D              swapChainExtent = {};
};
