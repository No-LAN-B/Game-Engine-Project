

#include "VulkanApp.h"

VulkanApp::VulkanApp() {
    // Constructor implementation
}

VulkanApp::~VulkanApp() {
    // Destructor implementation
}

// The reason for using std::vector<const char*> 
// validationLayers in code is because you need to store multiple validation layer names as a dynamic list of strings (character pointers).
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


// The vkCreateDebugUtilsMessengerEXT call requires a valid instance to have been created and vkDestroyDebugUtilsMessengerEXT must be called before the instance is destroyed.
// This currently leaves us unable to debug any issues in the vkCreateInstance and vkDestroyInstance calls.
// If you closely read the extension documentation, you'll see that there is a way to create a separate debug utils messenger specifically for those two function calls.
// It requires you to simply pass a pointer to a VkDebugUtilsMessengerCreateInfoEXT struct in the pNext extension field of VkInstanceCreateInfo.
// First extract population of the messenger create info into a separate function: this function is the populate debug messenger function lower down
VkResult VulkanApp::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}



void VulkanApp::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
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

VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

//  instance is the connection between your application and the Vulkan library and creating it involves specifying some details about your application to the driver.
void VulkanApp::initVulkan() {
    // Vulkan initialization logic here
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
}



// 
void VulkanApp::createSurface() {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

// The creation of a logical device involves specifying a bunch of details in structs again, of which the first one will be VkDeviceQueueCreateInfo. 
// This structure describes the number of queues we want for a single queue family. Right now we're only interested in a queue with graphics capabilities.
void VulkanApp::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // The next information to specify is the set of device features that we'll be using
    VkPhysicalDeviceFeatures deviceFeatures{};

    // With the previous two structures in place, we can start filling in the main VkDeviceCreateInfo structure.
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    // add pointers to the queue creation info and device features structs:
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    // enabledLayerCount and ppEnabledLayerNames fields of VkDeviceCreateInfo are ignored by up-to-date implementations. However, it is still a good idea to set them anyway to be compatible with older implementations:
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    // instantiate the logical device with a call to the appropriately named vkCreateDevice

    // The parameters are the physical device to interface with, the queue and usage info we just specified,
    // the optional allocation callbacks pointer and a pointer to a variable to store the logical device handle in. 
    // Similarly to the instance creation function, this call can return errors based on enabling non-existent extensions or specifying the desired usage of unsupported features.
    // the device is destroyed in the cleanup function
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
    // The queues are automatically created along with the logical device, but I don't have a handle to i nterface with them yet. 
    // Device queues are implicitly cleaned up when the device is destroyed, so we don't need to do anything in cleanup
    // vkGetDeviceQueue function to retrieve queue handles for each queue family.
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    // Because we're only creating a single queue from this family, we'll simply use index 0
}


// The graphics card that we'll end up selecting will be stored in a VkPhysicalDevice handle that is added as a new class member. 
// This object will be implicitly destroyed when the VkInstance is destroyed, so we won't need to do anything new in the cleanup function.
void VulkanApp::pickPhysicalDevice() {

    // The graphics card that we'll end up selecting will be stored in a VkPhysicalDevice handle that is added as a new class member.
    

    // Listing the graphics cards is very similar to listing extensions and starts with querying just the number.
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    // If there are 0 devices with Vulkan support then there is no point going further
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    // Otherwise we can now allocate an array to hold all of the VkPhysicalDevice handles
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }
    // check if any of the physical devices meet the requirements
    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

// Now we need to evaluate each of them and check if they are suitable for the operations we want to perform, because not all graphics cards are created equal :( :

// To evaluate the suitability of a device we can start by querying for some details.
// Basic device properties like the name, type and supported Vulkan version can be queried using vkGetPhysicalDeviceProperties

// he support for optional features like texture compression, 64 bit floats and multi viewport rendering (useful for VR) can be queried using vkGetPhysicalDeviceFeatures:

//bool VulkanApp::isDeviceSuitable(VkPhysicalDevice device) {
    // As an example, let's say we consider our application only usable for dedicated graphics cards that support geometry shaders.
    // Then the isDeviceSuitable function would look like this:
    //VkPhysicalDeviceProperties deviceProperties;
    //VkPhysicalDeviceFeatures deviceFeatures;
    //vkGetPhysicalDeviceProperties(device, &deviceProperties);
    //vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    //return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
    //return true;
//}


bool VulkanApp::isDeviceSuitable(VkPhysicalDevice device) {
    // use queue lookup function :)
    QueueFamilyIndices indices = findQueueFamilies(device);

    return indices.isComplete();
}

VulkanApp::QueueFamilyIndices VulkanApp::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    // The process of retrieving the list of queue families is exactly what you expect and uses vkGetPhysicalDeviceQueueFamilyProperties
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());


    // The VkQueueFamilyProperties struct contains some details about the queue family,
    // including the type of operations that are supported and the number of queues that can be created based on that family
    // I need to find at least one queue family that supports VK_QUEUE_GRAPHICS_BIT
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        // modified the findQueueFamilies function to look for a queue family that has the capability of presenting to our window surface.
        // The function to check for that is vkGetPhysicalDeviceSurfaceSupportKHR, 
        // which takes the physical device, queue family index and surface as parameters.
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

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

void setupDebugMessenger() {
    if (!enableValidationLayers) return;

}

void VulkanApp::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}


void VulkanApp::cleanup() {
    
    // If I want to see which call triggered a message, 
    // I can add a breakpoint to the message callback and look at the stack trace.

    if (enableValidationLayers) {
       DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    // Window surface destruction (Windows = KHR Cross Platform = glfw / normal one)
    vkDestroySurfaceKHR(instance, surface, nullptr);
    // the allocation and deallocation functions in Vulkan have an optional allocator callback that we'll ignore by passing nullptr to it
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();

    vkDestroyDevice(device, nullptr);

}




// to create an instance we'll first have to fill in a struct with some information about our application.
void VulkanApp::createInstance() {

    // Validation Layers for the instance
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }
    // many structs in Vulkan require you to explicitly specify the type in the sType member. 
    // This is also one of the many structs with a pNext member that can point to extension information in the future. \
    // We're using value initialization here to leave it as nullptr
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Modurr Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // A lot of information in Vulkan is passed through structs instead of function parameters 
    // we'll have to fill in one more struct to provide sufficient information for creating an instance

    // The first two parameters are straightforward.The next two layers specify the desired global extensions.
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Including Callback Function
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();


    // This struct should be passed to the vkCreateDebugUtilsMessengerEXT function to create the VkDebugUtilsMessengerEXT object. 
    // Unfortunately, because this function is an extension function, it is not automatically loaded. We have to look up its address ourselves using vkGetInstanceProcAddr. 
    // We're going to create our own proxy function that handles this in the background. which is the very top function
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

void VulkanApp::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}
void VulkanApp::setupDebugMessenger() {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);


    // The vkGetInstanceProcAddr function will return nullptr if the function couldn't be loaded. We can now call this function to create the extension object if it's available:
    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
        // The second to last parameter is again the optional allocator callback that we set to nullptr the other parameters are straightforward
    }
}
std::vector<const char*> VulkanApp::getRequiredExtensions(){

    // The extensions specified by GLFW are always required, but the debug messenger extension is conditionally added.
    // Note that I've used the VK_EXT_DEBUG_UTILS_EXTENSION_NAME macro here which is equal to the literal string "VK_EXT_debug_utils". 
    // Using this macro lets me avoid typos

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

// function checkValidationLayerSupport that checks if all of the requested layers are available
bool VulkanApp::checkValidationLayerSupport() {
    // uint = unsigned 32 bit int
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // Next, check if all of the layers in validationLayers exist in the availableLayers list. 
    // included <cstring> for strcmp.
    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;

}

// To set up a callback in the program to handle messages and the associated details,
// I have to set up a debug messenger with a callback using the VK_EXT_debug_utils extension

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanApp::debugCallback(

    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    // the pUserData parameter contains a pointer that was specified during the setup of the callback and allows you to pass your own data to it.
    void* pUserData) {
   // The first parameter specifies the severity of the message, which is one of the following flags:
   // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: Diagnostic message
   // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT : Informational message like the creation of a resource
   // VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT : Message about behavior that is not necessarily an error, but very likely a bug in your application
   // VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT : Message about behavior that is invalid and may cause crashes

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}





