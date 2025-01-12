#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

///////////////////
#include <iostream>
#include <cstring>
#include <optional>
#include <set>
#include <cstdint>
#include <limits> 
#include <algorithm> 
#include <fstream>

////////////////////////////////////////////
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
////////////////////////////////////////////

const std::vector<const char*> validationLayers = {
               "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class VulkanInstance {

public:
  
    void initVulkan();
    void initWindow();
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    //Platform agnostic window creation
    void createSurface();
    // Use validation layer for support for necessary queues and functionalities on the physical device
    bool checkValidationLayerSupport();
    // Create a main Vulkan instance using physical devices to create a logical device and then using the logical device
    void createInstance();
    //////
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    void pickPhysicalDevice();
    void createLogicalDevice();
    //////
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    //////
    void createSwapChain();
    void createImageViews();

    /// Graphics Pipeline - readFile is a helper to load SPIRV data into engine
    void createGraphicsPipeline();
    static std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    void createRenderPass();
    // Destroy all the initialised devices and structs created
    void cleanup();


    // VARIABLES ///////////////////////////////////
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    const int MAX_FRAMES_IN_FLIGHT = 2;

    /// THIS variable could be weird, check back on it
    uint32_t currentFrame = 0;
    bool framebufferResized = false;


    VkInstance instance;
    // The main game window
    GLFWwindow* window;
    // Graphics card/cards
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    // Logical device for vulkan
    VkDevice logicalDevice = VK_NULL_HANDLE;
    // Graphics queue
    VkQueue graphicsQueue;
    // Vulkan surface that talks to the platform specific surface
    VkSurfaceKHR surface;
    // presentation queue in the logical device
    VkQueue presentQueue;
    // Swapchain
    VkSwapchainKHR swapChain;
    // SwapChainImages
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;

    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    
    std::vector<VkFramebuffer> swapChainFramebuffers;
    void createFramebuffers();

    VkCommandPool commandPool;
    void createCommandPool();

    std::vector<VkCommandBuffer> commandBuffers;
    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    void createSyncObjects();

    void drawFrame();


    void recreateSwapChain();
    void cleanupSwapChain();

};

