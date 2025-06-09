#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// #include <vulkan/vulkan.h>
// import std::cerr
#include <iostream>
// import std::exception
#include <stdexcept>
// explicit import EXIT_SUCCESS/FAILURE
#include <cstdlib>
#include <vector>
#include <cstring>

const uint32_t WIDTH = 1024;
const uint32_t HEIGHT = 768;

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }
private:
    GLFWwindow* window;
    VkInstance instance;

    void initWindow() {
        // init glfw
        glfwInit();
        // tell GLFW not to create OpenGL context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // no re-sizable
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan(){
        createInstance();
    }

    void mainLoop() {
        while(!glfwWindowShouldClose(window)) {
            // check window related events, such as keypressing
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void createInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
        appInfo.pEngineName = "No Engine";
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

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        std::cout << "available extensions:\n";
        // challenge: check if glfw extensions are all supported
        bool isAllExtensionsSupported = false;
        for(int i = 0; i < glfwExtensionCount; ++i) {
            isAllExtensionsSupported = false;
            for(const auto& each : extensions) {
                if(strcmp(each.extensionName, glfwExtensions[i]) == 0) {
                        isAllExtensionsSupported = true;
                        break;
                }
            }           
            if(!isAllExtensionsSupported) {
                throw std::runtime_error("extensions not supported: " + std::string(glfwExtensions[i]));
            }
        }
        createInfo.enabledLayerCount = 0;
        if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }
};

int main() {
    HelloTriangleApplication app;
    try {
        app.run();
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}