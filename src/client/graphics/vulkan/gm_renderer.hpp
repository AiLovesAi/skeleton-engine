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

            VkRenderPass renderPass() const { return _swapChain->renderPass(); }
            float aspectRatio() const { return _swapChain->extentAspectRatio(); }
            VkCommandBuffer currentCommandBuffer() const { return _commandBuffers[_currentFrameIndex]; }
            int frameIndex() const { return _currentFrameIndex; }

            VkCommandBuffer beginFrame();
            void endFrame();
            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer) { vkCmdEndRenderPass(commandBuffer); }

        private:
            void createCommandBuffers();
            void freeCommandBuffers();
            void recreateSwapChain();
            
            GraphicsInstance& _graphicsInstance;
            GraphicsDevice& _graphicsDevice;
            Window& _window;

            std::unique_ptr<SwapChain> _swapChain;
            std::vector<VkCommandBuffer> _commandBuffers;
            VkViewport _viewport{};
            VkRect2D _scissor{};
            std::array<VkClearValue, 2> _clearValues{};
            VkCommandBufferBeginInfo _commandBufferInfo{};
            VkRenderPassBeginInfo _renderPassInfo{};

            uint32_t _currentImageIndex;
            int _currentFrameIndex;
    };
}
