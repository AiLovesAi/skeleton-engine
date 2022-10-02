#include "gm_renderer.hpp"

#include <common/logger/gm_logger.hpp>

#include <array>

namespace game {
     Renderer::Renderer(
        GraphicsInstance& instance,
        GraphicsDevice& device,
        Window& window
    ) : graphicsInstance_{instance}, graphicsDevice_{device}, window_{window} {
        viewport_.maxDepth = 1.0f;

        clearValues_[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues_[1].depthStencil = {1.0f, 0};
        
        commandBufferInfo_.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        renderPassInfo_.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo_.clearValueCount = static_cast<uint32_t>(clearValues_.size());
        renderPassInfo_.pClearValues = clearValues_.data();

        recreateSwapChain();
        createCommandBuffers();
    }

    Renderer::~Renderer() {
        freeCommandBuffers();
    }

    void Renderer::recreateSwapChain() {
        auto extent = window_.extent();
        
        while (extent.width == 0 || extent.height == 0) {
            glfwWaitEvents();
            extent = window_.extent();
        }

        vkDeviceWaitIdle(graphicsDevice_.device());
        
        if (swapChain_ == nullptr) {
            swapChain_ = std::make_unique<SwapChain>(graphicsInstance_, graphicsDevice_, extent);
        } else {
            // TODO Edit existing swapchain without setting present mode, etc.
            std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain_);
            swapChain_ = std::make_unique<SwapChain>(graphicsInstance_, graphicsDevice_, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*swapChain_.get())) {
                Logger::crash("Swap chain image or depth format has changed.");
            }
        }
        
        renderPassInfo_.renderPass = swapChain_->renderPass();
        renderPassInfo_.renderArea.extent = swapChain_->extent();
        viewport_.width = static_cast<float>(swapChain_->width());
        viewport_.height = static_cast<float>(swapChain_->height());
        scissor_.extent = swapChain_->extent();
    }

    void Renderer::createCommandBuffers() {
        commandBuffers_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = graphicsDevice_.commandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());

        vkAllocateCommandBuffers(graphicsDevice_.device(), &allocInfo, commandBuffers_.data());
    }

    void Renderer::freeCommandBuffers() {
        vkFreeCommandBuffers(graphicsDevice_.device(), graphicsDevice_.commandPool(),
            static_cast<uint32_t>(commandBuffers_.size()), commandBuffers_.data());
        commandBuffers_.clear();
    }

    VkCommandBuffer Renderer::beginFrame() {
        swapChain_->acquireNextImage(&currentImageIndex_);

        auto commandBuffer = currentCommandBuffer();
        vkBeginCommandBuffer(commandBuffer, &commandBufferInfo_);
        return commandBuffer;
    }

    void Renderer::endFrame() {
        auto commandBuffer = currentCommandBuffer();
        vkEndCommandBuffer(commandBuffer);

        swapChain_->submitCommandBuffers(&commandBuffer, &currentImageIndex_);
        if (window_.wasResized()) {
            window_.resetWindowResizedFlag();
            recreateSwapChain();
        }

        currentFrameIndex_ = (currentFrameIndex_ + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }
    
    void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        renderPassInfo_.framebuffer = swapChain_->frameBuffer(currentImageIndex_);

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo_, VK_SUBPASS_CONTENTS_INLINE);        
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport_);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor_);
    }
}
