cmake_minimum_required(VERSION 3.14)
project(GameEngine)

# ——————————————————————————————————————————————
# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ——————————————————————————————————————————————
# Source & header files
set(SRC_FILES
    src/main.cpp
    src/VulkanApp.cpp
    src/Window.cpp
    src/Device.cpp
    src/Pipeline.cpp
    src/SwapChain.cpp
    src/DebugUtils.cpp
    src/Utils.cpp
)

set(HEADER_FILES
    src/VulkanApp.h
    src/Window.h
    src/Device.h
    src/Pipeline.h
    src/SwapChain.h
    src/DebugUtils.h
    src/Utils.h
)

# ——————————————————————————————————————————————
# Shader files (inputs)
file(GLOB SHADERS_TO_COMPILE
  "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.vert"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.frag"
)

# (Optional) expose them in VS under a “Shaders” filter
source_group("Shaders" FILES ${SHADERS_TO_COMPILE})

# ——————————————————————————————————————————————
# Executable
add_executable(GameEngine
  ${SRC_FILES}
  ${HEADER_FILES}
)

# Attach shaders to the GameEngine project so VS will show a “Shaders” filter
set_source_files_properties(${SHADERS_TO_COMPILE}
  PROPERTIES HEADER_FILE_ONLY TRUE
)
target_sources(GameEngine
  PRIVATE
    ${SHADERS_TO_COMPILE}
)

# ——————————————————————————————————————————————
# Shader‐compile integration

# 1) Where is the script?
set(SHADER_COMPILER
  "${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile.bat"
)

# 2) Where to put the compiled SPIR-V
set(GENERATED_SPV_DIR
  "${CMAKE_CURRENT_BINARY_DIR}/shaders_spv"
)

# 3) Batch‐compile step whenever any GLSL/HLSL changes
add_custom_target(CompileShaders
  COMMAND "${SHADER_COMPILER}"
          "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders"   # input folder
          "${GENERATED_SPV_DIR}"                      # output folder
  DEPENDS ${SHADERS_TO_COMPILE}
  COMMENT "Compiling GLSL/HLSL → SPIR-V via compile.bat"
  VERBATIM
)

# 4) Engine builds only after shaders are compiled
add_dependencies(GameEngine CompileShaders)

# ——————————————————————————————————————————————
# Expose scripts folder in the VS solution

# 1) Grab all the .bat (or any) files under scripts/
file(GLOB SCRIPT_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/scripts/*.bat"
  "${CMAKE_CURRENT_SOURCE_DIR}/scripts/*.ps1"  # if you ever add PowerShell helpers
)

# 2) Make a VS “Scripts” filter and mark them non-C++
source_group("Scripts" FILES ${SCRIPT_FILES})
set_source_files_properties(
  ${SCRIPT_FILES}
  PROPERTIES HEADER_FILE_ONLY TRUE
)

# 3) Attach them to project
target_sources(GameEngine
  PRIVATE
    ${SCRIPT_FILES}
)

# ——————————————————————————————————————————————
# Find & link libraries
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(GameEngine
  PRIVATE
    Vulkan::Vulkan
    glfw
)

# ——————————————————————————————————————————————
# Include dirs
target_include_directories(GameEngine PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)