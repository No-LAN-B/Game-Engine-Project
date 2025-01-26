cmake_minimum_required(VERSION 3.14)
project(GameEngine)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
set(SRC_FILES
    src/main.cpp
    src/VulkanApp.cpp
    src/Window.cpp
    src/Device.cpp
    src/Pipeline.cpp
)

set(HEADER_FILES
    src/VulkanApp.h
    src/Window.h
    src/Device.h
    src/Pipeline.h
)

# Add executable
add_executable(GameEngine ${SRC_FILES} ${HEADER_FILES})

# Find Vulkan and GLFW
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)


# Link Vulkan
target_link_libraries(GameEngine Vulkan::Vulkan)

# Link libraries
target_link_libraries(GameEngine Vulkan::Vulkan glfw)

# Include directories
target_include_directories(GameEngine PRIVATE src)