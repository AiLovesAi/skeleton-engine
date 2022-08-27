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

            VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }
            float getAspectRatio() const { return swapChain->extentAspectRatio(); }
            bool isFrameInProgress() const { return isFrameStarted; }

            VkCommandBuffer getCurrentCommandBuffer() const {
                assert(isFrameStarted && "Cannot get command buffer when frame is not in progress.");
                return commandBuffers[currentFrameIndex];
            }

            int getFrameIndex() const {
                assert(isFrameStarted && "Cannot get frame index when frame is not in progress.");
                return currentFrameIndex;
            }

            VkCommandBuffer beginFrame();
            void endFrame(Camera *camera);
            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

        private:
            void createCommandBuffers();
            void freeCommandBuffers();
            void recreateSwapChain();
            
            GraphicsInstance* graphicsInstance;
            GraphicsDevice* graphicsDevice;
            Window* window;
            std::unique_ptr<SwapChain> swapChain;
            std::vector<VkCommandBuffer> commandBuffers;

            uint32_t currentImageIndex;
            int currentFrameIndex;
            bool isFrameStarted = false;
    };
}
