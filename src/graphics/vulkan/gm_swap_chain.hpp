#pragma once

#include "gm_graphics_device.hpp"
#include "gm_graphics_instance.hpp"

#include <memory>
#include <string>
#include <vector>

namespace game {
    class SwapChain {
        public:
            static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

            SwapChain(GraphicsInstance* instance, GraphicsDevice* device, VkExtent2D windowExtent);
            SwapChain(GraphicsInstance* instance, GraphicsDevice* device, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);
            ~SwapChain();

            SwapChain(const SwapChain &) = delete;
            SwapChain &operator=(const SwapChain &) = delete;
            SwapChain(SwapChain&&) = default;
            SwapChain &operator=(SwapChain&&) = default;

            VkFramebuffer frameBuffer(int index) { return framebuffers_[index]; }
            VkRenderPass renderPass() { return renderPass_; }
            VkImageView imageView(int index) { return imageViews_[index]; }
            size_t imageCount() { return images_.size(); }
            VkFormat imageFormat() { return imageFormat_; }
            VkExtent2D extent() { return extent_; }
            uint32_t width() { return extent_.width; }
            uint32_t height() { return extent_.height; }

            float extentAspectRatio() {
                return static_cast<float>(extent_.width) / static_cast<float>(extent_.height);
            }
            VkFormat findDepthFormat();

            VkResult acquireNextImage(uint32_t* imageIndex);
            VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

            bool compareSwapFormats(const SwapChain& swapChain) const {
                return swapChain.depthFormat_ == depthFormat_ &&
                    swapChain.imageFormat_ == imageFormat_;
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
            VkFormat imageFormat_;
            VkFormat depthFormat_;
            VkExtent2D extent_;

            std::vector<VkFramebuffer> framebuffers_;
            VkRenderPass renderPass_;

            std::vector<VkImage> depthImages_;
            std::vector<VkDeviceMemory> depthImageMemorys_;
            std::vector<VkImageView> depthImageViews_;
            std::vector<VkImage> images_;
            std::vector<VkImageView> imageViews_;

            GraphicsInstance* graphicsInstance_;
            GraphicsDevice* graphicsDevice_;
            VkExtent2D windowExtent_;

            VkSwapchainKHR swapChain_;
            std::shared_ptr<SwapChain> oldSwapChain_;

            std::vector<VkSemaphore> imageAvailableSemaphores_;
            std::vector<VkSemaphore> renderFinishedSemaphores_;
            std::vector<VkFence> inFlightFences_;
            std::vector<VkFence> imagesInFlight_;
            size_t currentFrame_ = 0;
    };
}
