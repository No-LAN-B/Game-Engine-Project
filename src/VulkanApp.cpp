#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// GLFW will include its own definitions and automatically load the Vulkan header with it

#include <iostream> // Debugging Errors
#include <stdexcept> // Debugging Errors
#include <cstdlib> // Exit Success and Exit Failure


// use constants instead of hardcoded width and height numbers because we'll be referring to these values a couple of times in the future.
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

GLFWwindow* window; // private class member to store a reference to window

// Store Vulkan objects as private class members and add functions to initiate each of them called from the initVulkan function
class VulkanApp { 
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    void initWindow() {
        glfwInit(); // initializes the GLFW library.
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // GLFW was originally designed to create an OpenGL context, we need to tell it to not create an OpenGL context
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);  // resized windows takes special care that we'll look into later, disable it for now  

        
        // width, height, Window Title, Optional specifier to a monitor to open the window on, last parameter is only relevant to OpenGL.
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

        



    }

    // RAII <- From Memory Header

    // implement automatic resource management by writing C++ classes that acquire Vulkan objects in their constructor and release them in their destructor
    // or by providing a custom deleter to either std::unique_ptr or std::shared_ptr

private:
    void initVulkan() {

    }


    // enter the main loop to start rendering frames
    // function to include a loop that iterates until the window is closed
    void mainLoop() { 
        // Event Loop For The Window
        while (!glfwWindowShouldClose(window)) { 
            glfwPollEvents();
        }
    }

    // Just like each chunk of memory allocated with malloc requires a call to free, 
    // every Vulkan object that we create needs to be explicitly destroyed when we no longer need it.
    
    // Once the window is closed and mainLoop returns
    // deallocate the resources we've used -> in the cleanup function.
    void cleanup() {
        glfwDestroyWindow(window);

        glfwTerminate();
    }
};

int main() {
    VulkanApp app;

    // If any kind of fatal error occurs during execution then we'll throw a 
    // std::runtime_error exception with a descriptive message 
    // which will propagate back to the main function and be printed to the command prompt

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}