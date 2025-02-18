

#include "VulkanApp.h"

VulkanApp::VulkanApp() {
    // Constructor implementation
}

VulkanApp::~VulkanApp() {
    // Destructor implementation
}

void VulkanApp::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void VulkanApp::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}


//  instance is the connection between your application and the Vulkan library and creating it involves specifying some details about your application to the driver.
void VulkanApp::initVulkan() {
    // Vulkan initialization logic here
    createInstance();
}

void VulkanApp::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void VulkanApp::cleanup() {

    // the allocation and deallocation functions in Vulkan have an optional allocator callback that we'll ignore by passing nullptr to it
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
}

// to create an instance we'll first have to fill in a struct with some information about our application.
void VulkanApp::createInstance() {

    // many structs in Vulkan require you to explicitly specify the type in the sType member. 
    // This is also one of the many structs with a pNext member that can point to extension information in the future. \
    // We're using value initialization here to leave it as nullptr
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // A lot of information in Vulkan is passed through structs instead of function parameters 
    // we'll have to fill in one more struct to provide sufficient information for creating an instance

    // The first two parameters are straightforward.The next two layers specify the desired global extensions.
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // GLFW has a handy built - in function that returns the extension(s) it needs to do that which we can pass to the struct :
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    // The last two members of the struct determine the global validation layers to enable
    createInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}



