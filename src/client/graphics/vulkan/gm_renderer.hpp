#pragma once

#include "gm_graphics_device.hpp"
#include "gm_graphics_instance.hpp"
#include "gm_swap_chain.hpp"
#include "../window/gm_window.hpp"

namespace game {
    class Renderer {
        public:
            Renderer(GraphicsInstance& instance, GraphicsDevice& device, Window& window);
            ~Renderer();

            Renderer(const Renderer &) = delete;
            Renderer &operator=(const Renderer &) = delete;
            Renderer(const Renderer &&) = delete;
            Renderer &operator=(const Renderer &&) = delete;

            VkRenderPass renderPass() const { return swapChain_->renderPass(); }
            float aspectRatio() const { return swapChain_->extentAspectRatio(); }
            VkCommandBuffer currentCommandBuffer() const { return commandBuffers_[currentFrameIndex_]; }
            int frameIndex() const { return currentFrameIndex_; }

            VkCommandBuffer beginFrame();
            void endFrame();
            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer) { vkCmdEndRenderPass(commandBuffer); }

        private:
            void createCommandBuffers();
            void freeCommandBuffers();
            void recreateSwapChain();
            
            GraphicsInstance& graphicsInstance_;
            GraphicsDevice& graphicsDevice_;
            Window& window_;

            std::unique_ptr<SwapChain> swapChain_;
            std::vector<VkCommandBuffer> commandBuffers_;
            VkViewport viewport_{};
            VkRect2D scissor_{};
            std::array<VkClearValue, 2> clearValues_{};
            VkCommandBufferBeginInfo commandBufferInfo_{};
            VkRenderPassBeginInfo renderPassInfo_{};

            uint32_t currentImageIndex_;
            int currentFrameIndex_;
    };
}
