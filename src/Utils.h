// src/Utils.h
#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

// Read the contents of a binary file into a byte buffer.
std::vector<char> readFile(const std::string& filename);

// Get required instance extensions from GLFW, plus debug utils if enabled.
std::vector<const char*> getRequiredExtensions(bool enableValidation);
