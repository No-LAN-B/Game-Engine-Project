#include <iostream>
#include "VulkanApp.h"

int main() {
    VulkanApp app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
