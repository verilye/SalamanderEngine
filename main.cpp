#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <cstring>
#include <optional>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

// The main Vulkan instance
VkInstance instance;
// The main game window
GLFWwindow* window;
// Graphics card/cards
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
// Logical device for vulkan
VkDevice logicalDevice = VK_NULL_HANDLE;

// Use validation layer for support for necessary queues and functionalities on the physical device
bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

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


// Create a main Vulkan instance using physical devices to create a logical device and then using the logical device
void createInstance() {

    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Salamander test";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Salamander";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }

}

struct QueueFamilyIndices{
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

// Queue families are operations the GPU supports
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    std::optional<uint32_t> graphicsFamily;
    
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    // WE'RE looking for a queue family that has the propery VK_QUEUE_GRAPHICS_BIT
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

bool isDeviceSuitable(VkPhysicalDevice device) {

    QueueFamilyIndices indices = findQueueFamilies(device);

    // EXAMPLE RETURN USAGE OF THIS FUNCTION
    //VkPhysicalDeviceProperties deviceProperties;
    //vkGetPhysicalDeviceProperties(device, &deviceProperties);
    //
    ////Get device features - ie texture compression, 64 bit floats, multi viewport rendering
    //VkPhysicalDeviceFeatures deviceFeatures;
    //vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    /*return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
        deviceFeatures.geometryShader;*/

    // For the purposes of getting everything working, take the first GPU we find for now
    return indices.isComplete();
}

void pickPhysicalDevice() {

    uint32_t deviceCount = 0;
    if (vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr) == VK_INCOMPLETE) {
        throw std::runtime_error("enumeratePhysicalDevices failed");
    }
    if (deviceCount == 0) {
        throw std::runtime_error("No gpus with vulkan support found");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);

    if (vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()) == VK_INCOMPLETE) {
        throw std::runtime_error("enumeratePhysicalDevices failed");
    }

    // We want to evaluate if the GPU selected is suitable for everything we want to achieve
    for (const auto& device : devices) {

        // A good idea is to rate each device based on criteria and return the one with the 
        // highest score, for example highest power and most featuers needed
        // Also a good idea to let people select a device from a dropdown or something incase
        // the automatic selection yields imperfect results
        if (isDeviceSuitable(device)){
            physicalDevice = device;
            break;
        }
    }

    // Make sure to compare variables to ERROR CODES instead of NULL or 0 if possible
    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to bind physical device to vulkan instance");
    }
    
}

// Use physical devices selected by previous functions to create the logical device for the engine
void createLogicalDevice() {

    // Create outline of queues obtained from the gpu add them to the logical device
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    // Functionality we are including in the logical device 
    VkPhysicalDeviceFeatures deviceFeatures{};
    // NOTE - functionalities are abstracted features, queues are the channels we submit commands down on the cpu
    // queues can be device specfic and dont map onto features directly

    VkDeviceCreateInfo logicalDeviceChain{};
    logicalDeviceChain.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    logicalDeviceChain.pQueueCreateInfos = &queueCreateInfo;
    logicalDeviceChain.queueCreateInfoCount = 1;
    logicalDeviceChain.pEnabledFeatures = &deviceFeatures;
        
    // Call this to finally create the logical device
    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device");
    }
    //CHECK THE OUTPUT OF THIS TO SEE IF IT WAS SUCCESSFUL

   

    // A logical device can be created that connects to one or more physical devices by adding a VkDeviceGroupDeviceCreateInfo
    // structure to the pNext chain of VkDeviceCreateInfo
}

void initWindow() {

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void initVulkan() {

    createInstance();
    pickPhysicalDevice();
    createLogicalDevice();

}

void cleanup() {
    vkDestroyInstance(instance, nullptr);
    vkDestroyDevice(logicalDevice, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();
}

int main() {
   
    // System setup
    initWindow();
    initVulkan();
    createInstance();
    

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    cleanup();

    return 0;
}