#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer()
{}

VulkanRenderer::~VulkanRenderer()
{}

int VulkanRenderer::init(GLFWwindow *newWindow)
{
    window = newWindow;

    try
    {
        createInstance();
        getPhysicalDevice();
        createLogicalDevice();
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
    }


    return 0;
}

void VulkanRenderer::cleanup()
{
    vkDestroyInstance(instance, nullptr);
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);
}

void VulkanRenderer::createInstance()
{
    // VkApplication Info
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "OnyxVK";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName = "NULL";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // VkInstance Info
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Get VKExtensions
    std::vector<const char*> instanceExtensions = std::vector<const char*>();
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (size_t i = 0; i < glfwExtensionCount; i++)
    {
        instanceExtensions.push_back(glfwExtensions[i]);
    }

    if (!checkInstanceExtensionSupport(&instanceExtensions))
    {
        throw std::runtime_error("VkInstance does not support required extensions");
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();

    //TODO: Create Validation Layers
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;

    // Create VkInstance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create VkInstance");
    }
}

void VulkanRenderer::createLogicalDevice()
{
    // Get Queue Family Indices
    QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

    // Required Queues
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
    queueCreateInfo.queueCount = 1;

    float priority = 1.0f;
    queueCreateInfo.pQueuePriorities = &priority;
    
    // Create Logical Device
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    // Create Logical Device
    VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Logical Create");
    }

    // Handle Queues
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
}

void VulkanRenderer::getPhysicalDevice()
{
    // Get Physical Devices count
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    // Check if Physical Devices found
    if (deviceCount == 0)
    {
        throw std::runtime_error("No devices found supporting Vulkan Instance");
    }

    // List Physical Devices
    std::vector<VkPhysicalDevice> deviceList(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

    // Get First Suitable Device
    for (const auto &device : deviceList)
    {
        if (checkDeviceSuitable(device))
        {
            mainDevice.physicalDevice = device;
            break;
        }
    }
}

bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char*> *checkExtensions)
{
    // Get number of extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    // Create VkExtensionProperties List
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    // Check if available
    for(const auto &checkExtension : *checkExtensions)
    {
        bool hasExtension = false;
        
        for (const auto &extension : extensions)
        {
            if (strcmp(checkExtension, extension.extensionName))
            {
                hasExtension = true;
                break;
            }
        }

        if (!hasExtension)
        {
            return false;
        }
    }

    return true;
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
    /*
    // Device Info
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    // Device Capabilities Info
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    */

    QueueFamilyIndices indices = getQueueFamilies(device);

    return indices.isValid();
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    // Get Queue Family Property Info
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

    // Check required queue exists
    int i = 0;
    for (const auto &queueFamily : queueFamilyList)
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        if (indices.isValid())
        {
            break;
        }

        i++;
    }

    return indices;
}