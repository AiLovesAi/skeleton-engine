#pragma once

#include "gm_graphics_device.hpp"
#include "gm_graphics_instance.hpp"

#include <common/headers/float.hpp>

#include <memory>
#include <string>
#include <vector>

namespace game {
    class SwapChain {
        public:
            static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

            SwapChain(GraphicsInstance& instance, GraphicsDevice& device, VkExtent2D windowExtent);
            SwapChain(GraphicsInstance& instance, GraphicsDevice& device, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);
            ~SwapChain();

            SwapChain(const SwapChain &) = delete;
            SwapChain &operator=(const SwapChain &) = delete;
            SwapChain(SwapChain&&) = default;
            SwapChain &operator=(SwapChain&&) = default;

            VkFramebuffer frameBuffer(int index) { return _framebuffers[index]; }
            VkRenderPass renderPass() { return _renderPass; }
            VkImageView imageView(int index) { return _imageViews[index]; }
            size_t imageCount() { return _images.size(); }
            VkFormat imageFormat() { return _imageFormat; }
            VkExtent2D extent() { return _extent; }
            uint32_t width() { return _extent.width; }
            uint32_t height() { return _extent.height; }

            float32_t extentAspectRatio() {
                return static_cast<float32_t>(_extent.width) / static_cast<float32_t>(_extent.height);
            }
            VkFormat findDepthFormat() {
                return _graphicsDevice.findSupportedFormat(
                    {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
                );
            }

            VkResult acquireNextImage(uint32_t* imageIndex);
            VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

            bool compareSwapFormats(const SwapChain& swapChain) const {
                return swapChain._depthFormat == _depthFormat &&
                    swapChain._imageFormat == _imageFormat;
            }

        private:
            // Functions
            void init();
            void createSwapChain();
            void createImageViews();
            void createDepthResources();
            void createRenderPass();
            void createFramebuffers();
            void createSyncObjects();

            VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
            VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
            VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

            // Variables
            VkFormat _imageFormat;
            VkFormat _depthFormat;
            VkExtent2D _extent;

            std::vector<VkFramebuffer> _framebuffers;
            VkRenderPass _renderPass;

            std::vector<VkImage> depthImages_;
            std::vector<VkDeviceMemory> _depthImageMemorys;
            std::vector<VkImageView> _depthImageViews;
            std::vector<VkImage> _images;
            std::vector<VkImageView> _imageViews;

            GraphicsInstance& _graphicsInstance;
            GraphicsDevice& _graphicsDevice;
            VkExtent2D _windowExtent;

            VkSwapchainKHR _swapChain;
            std::shared_ptr<SwapChain> _oldSwapChain;

            std::vector<VkSemaphore> _imageAvailableSemaphores;
            std::vector<VkSemaphore> _renderFinishedSemaphores;
            std::vector<VkFence> _inFlightFences;
            std::vector<VkFence> _imagesInFlight;
            size_t _currentFrame = 0;
    };
}
