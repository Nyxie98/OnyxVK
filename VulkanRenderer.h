#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <iostream>
#include <string.h>

#include "Utils.h"

class VulkanRenderer
{
public:
    VulkanRenderer();

    int init(GLFWwindow *newWindow);
    void cleanup();

    ~VulkanRenderer();

private:
    GLFWwindow *window;

    // VKComponents
    VkInstance instance;
    struct {
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
    } mainDevice;
    VkQueue graphicsQueue;

    // VKFunctions
    void createInstance();
    void createLogicalDevice();

    void getPhysicalDevice();

    bool checkInstanceExtensionSupport(std::vector<const char*> *checkExtensions);
    bool checkDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
};