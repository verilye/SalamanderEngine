#include "VulkanInstance.h"

// For cleanliness and obfuscation of boilerplate, wrapping vulkan initialisation
// in a class
VulkanInstance vulkanInstance;

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
    vulkanInstance.createInstance();
    
    mainLoop();

    vulkanInstance.cleanup();

    return 0;
}