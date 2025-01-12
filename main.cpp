#include "VulkanInstance.h"

// For cleanliness and obfuscation of boilerplate, wrapping vulkan initialisation
// in a class
VulkanInstance vulkanInstance;




// TODO - seperate Vulkan functionality out into different files
// one for main application
// device management
// swapchain management
// command buffers
// shaders and pipelines
// synchronisation
// utilities

void mainLoop() {

    while (!glfwWindowShouldClose(vulkanInstance.window)) {
        glfwPollEvents();
        vulkanInstance.drawFrame();
    }

    vkDeviceWaitIdle(vulkanInstance.logicalDevice);
}

int main() {
   
    // Initialise Systems
    vulkanInstance.initWindow();
    vulkanInstance.initVulkan();
    
    mainLoop();

    vulkanInstance.cleanup();

    return 0;
}