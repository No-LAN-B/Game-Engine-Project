@echo off
REM --------------------------------------------------------
REM compile-shaders.bat: compile two hard-coded shaders
REM Usage: just double-click this .bat in the same folder
REM --------------------------------------------------------

REM 1) Make sure VULKAN_SDK is set
if not defined VULKAN_SDK (
  echo ERROR: VULKAN_SDK environment variable not set!
  echo Please install the Vulkan SDK and define VULKAN_SDK.
  pause
  exit /b 1
)

REM 2) Define the compiler binary (you can swap in glslangValidator.exe -V if you prefer)
set GLSL_COMPILER=C:\VulkanSDK\1.4.304.0\Bin\glslc.exe

REM 3) Compile each shader
echo Compiling shader.vert to vert.spv
"C:\VulkanSDK\1.4.304.0\Bin\glslc.exe" -c "C:\GitHub\Game-Engine-Project\src\shaders\shader.vert" -o "C:\GitHub\Game-Engine-Project\build\shaders_spv\vert.spv"
if errorlevel 1 (
  echo ** ERROR: failed to compile shader.vert
  pause
  exit /b 1
)

echo Compiling shader.frag to frag.spv
"C:\VulkanSDK\1.4.304.0\Bin\glslc.exe" -c "C:\GitHub\Game-Engine-Project\src\shaders\shader.frag" -o "C:\GitHub\Game-Engine-Project\build\shaders_spv\frag.spv"
if errorlevel 1 (
  echo ** ERROR: failed to compile shader.frag
  pause
  exit /b 1
)

echo.
echo All shaders compiled successfully!
pause