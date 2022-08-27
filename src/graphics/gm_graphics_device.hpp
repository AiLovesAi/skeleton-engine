#pragma once

#include "gm_graphics_instance.hpp"
#include "gm_window.hpp"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace game {
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        uint32_t graphicsFamily;
        uint32_t presentFamily;
        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;
        bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
    };

    class GraphicsDevice {
        public:
            // Constructors
            GraphicsDevice(GraphicsInstance* graphicsInstance);
            ~GraphicsDevice();

            GraphicsDevice(const GraphicsDevice &) = delete;
            GraphicsDevice &operator=(const GraphicsDevice &) = delete;
            GraphicsDevice(GraphicsDevice &&) = delete;
            GraphicsDevice &operator=(GraphicsDevice &&) = delete;
            
            // Functions
            VkCommandPool getCommandPool() { return commandPool; }
            VkDevice getDevice() { return device; }
            VkQueue getGraphicsQueue() { return graphicsQueue; }
            VkQueue getPresentQueue() { return presentQueue; }

            SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }
            uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
            QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }

            // Variables
            VkPhysicalDeviceProperties properties;

        private:
            // Functions
            void pickPhysicalDevice();
            void createLogicalDevice();
            void createCommandPool();

            int rateDeviceSuitability(const VkPhysicalDevice& device);
            bool isDeviceSuitable(const VkPhysicalDevice& device, const VkPhysicalDeviceFeatures& supportedFeatures);
            QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device);
            bool checkDeviceExtensionSupport(const VkPhysicalDevice& device);
            SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device);
            
            // Variables
            GraphicsInstance* instance;

            VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
            VkCommandPool commandPool;

            VkDevice device;
            VkQueue graphicsQueue;
            VkQueue presentQueue;

            const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    };
}
