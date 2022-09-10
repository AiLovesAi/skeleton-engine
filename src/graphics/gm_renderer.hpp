#pragma once

#include "gm_camera.hpp"
#include "gm_graphics_device.hpp"
#include "gm_graphics_instance.hpp"
#include "gm_swap_chain.hpp"
#include "gm_window.hpp"

namespace game {
    class Renderer {
        public:
            Renderer(GraphicsInstance* instance, GraphicsDevice* device, Window* window);
            ~Renderer();

            Renderer(const Renderer &) = delete;
            Renderer &operator=(const Renderer &) = delete;

            VkRenderPass renderPass() const { return swapChain_->renderPass(); }
            float aspectRatio() const { return swapChain_->extentAspectRatio(); }
            bool isFrameInProgress() const { return isFrameStarted_; }

            VkCommandBuffer currentCommandBuffer() const {
                assert(isFrameStarted_ && "Cannot get command buffer when frame is not in progress.");
                return commandBuffers_[currentFrameIndex_];
            }

            int frameIndex() const {
                assert(isFrameStarted_ && "Cannot get frame index when frame is not in progress.");
                return currentFrameIndex_;
            }

            VkCommandBuffer beginFrame();
            void endFrame(Camera *camera);
            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

        private:
            void createCommandBuffers();
            void freeCommandBuffers();
            void recreateSwapChain();
            
            GraphicsInstance* graphicsInstance_;
            GraphicsDevice* graphicsDevice_;
            Window* window_;
            std::unique_ptr<SwapChain> swapChain_;
            std::vector<VkCommandBuffer> commandBuffers_;

            uint32_t currentImageIndex_;
            int currentFrameIndex_;
            bool isFrameStarted_ = false;
    };
}
