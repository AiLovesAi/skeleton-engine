#include "gm_graphics_instance.hpp"

#include <common/gm_core.hpp>
#include <common/data/file/gm_logger.hpp>
#include <common/headers/string.hpp>

#include <unordered_set>

namespace game {
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData
    ) {
        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
                Logger::log(LOG_INFO, FormatString::formatString("Validation layer info: %s", pCallbackData->pMessage));
            } break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
                Logger::log(LOG_ERR, FormatString::formatString("Validation layer caught error: %s", pCallbackData->pMessage));
            } break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
                Logger::log(LOG_WARN, FormatString::formatString("Validation layer caught warning: %s", pCallbackData->pMessage));
            } break;
            default:
                break;
        }
        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDebugUtilsMessengerEXT *pDebugMessenger
    ) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance,
            "vkCreateDebugUtilsMessengerEXT"
        );

        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks *pAllocator
    ) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance,
            "vkDestroyDebugUtilsMessengerEXT"
        );
        
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    GraphicsInstance::GraphicsInstance(Window& window) : _window{window} {
        createInstance();
        setupDebugMessenger();
        window.createWindowSurface(_instance, &_surface);
    }
    
    GraphicsInstance::~GraphicsInstance() {
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(_instance, _surface, nullptr);
        vkDestroyInstance(_instance, nullptr);
    }

    void GraphicsInstance::createInstance() {
        if (checkValidationLayerSupport()) {
            Logger::log(LOG_INFO, "Validation layers enabled.");
        }

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = Core::TITLE.get();
        appInfo.applicationVersion = 0;
        appInfo.pEngineName = "Vulkan";
        appInfo.engineVersion = 0;
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
            Logger::crash("Failed to create Vulkan instance instance.");
        }
        
        checkGFLWHasRequiredInstanceExtensions();
    }

    bool GraphicsInstance::checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *layerName : validationLayers) {
            bool layerFound = false;

            for (const auto &layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

#ifdef NDEBUG
        enableValidationLayers = true;
#endif
        return true;
    }

    std::vector<const char *> GraphicsInstance::getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void GraphicsInstance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    void GraphicsInstance::checkGFLWHasRequiredInstanceExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        StringBuffer msg{"Available Vulkan extensions:"};
        std::unordered_set<std::string> available;
        for (const auto &extension : extensions) {
            msg.append("\n\t");
            msg.append(extension.extensionName);
            available.insert(extension.extensionName);
        }
        Logger::log(LOG_INFO, msg.str());
        msg.clear();

        auto requiredExtensions = getRequiredExtensions();
        msg.append("Required extensions:");
        for (const auto &required : requiredExtensions) {
            msg.append("\n\t");
            msg.append(required);
            if (available.find(required) == available.end()) {
                Logger::crash("Missing required GLFW extension.");
            }
        }
        Logger::log(LOG_INFO, msg.str());
    }

    void GraphicsInstance::setupDebugMessenger() {
    if (!enableValidationLayers) return;
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);
        if (CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS) {
            Logger::crash("Failed to set up debug messenger.");
        }
    }
}
