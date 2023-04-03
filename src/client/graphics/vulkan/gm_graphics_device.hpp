#pragma once

#include "gm_graphics_instance.hpp"
#include "../window/gm_window.hpp"

#include <vector>

namespace game {
    // Types
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
            GraphicsDevice(GraphicsInstance& graphicsInstance);
            ~GraphicsDevice();

            GraphicsDevice(const GraphicsDevice &) = delete;
            GraphicsDevice &operator=(const GraphicsDevice &) = delete;
            GraphicsDevice(GraphicsDevice &&) = delete;
            GraphicsDevice &operator=(GraphicsDevice &&) = delete;
            
            // Functions
            VkCommandPool commandPool() { return _commandPool; }
            VkDevice device() { return _device; }
            VkQueue graphicsQueue() { return _graphicsQueue; }
            VkQueue presentQueue() { return _presentQueue; }

            SwapChainSupportDetails swapChainSupport() { return querySwapChainSupport(physicalDevice_); }
            QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice_); }

            uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
            VkFormat findSupportedFormat(
                const std::vector<VkFormat> &candidates,
                VkImageTiling tiling,
                VkFormatFeatureFlags features
            );
            void createBuffer(
                const VkDeviceSize size,
                const VkBufferUsageFlags& usage,
                const VkMemoryPropertyFlags& properties,
                VkBuffer& buffer,
                VkDeviceMemory& bufferMemory
            );
            VkCommandBuffer beginSingleTimeCommands();
            void endSingleTimeCommands(VkCommandBuffer commandBuffer);
            void copyBuffer(const VkBuffer& srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
            void copyBufferToImage(
                const VkBuffer& buffer,
                const VkImage& image,
                uint32_t width,
                uint32_t height,
                uint32_t layerCount
            );
            void createImageWithInfo(
                const VkImageCreateInfo& imageInfo,
                const VkMemoryPropertyFlags& properties,
                VkImage& image,
                VkDeviceMemory& imageMemory
            );

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
            GraphicsInstance& _graphicsInstance;

            VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
            VkCommandPool _commandPool;

            VkDevice _device;
            VkQueue _graphicsQueue;
            VkQueue _presentQueue;

            const std::vector<const char *> _deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    };
}
