#include "gm_renderer.hpp"

#include <util/logger/gm_logger.hpp>

#include <array>

namespace game {
     Renderer::Renderer(
        GraphicsInstance& instance,
        GraphicsDevice& device,
        Window& window
    ) : graphicsInstance_{instance}, graphicsDevice_{device}, window_{window} {
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
    }

    void Renderer::createCommandBuffers() {
        commandBuffers_.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = graphicsDevice_.commandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());

        if (vkAllocateCommandBuffers(graphicsDevice_.device(), &allocInfo, commandBuffers_.data()) != VK_SUCCESS) {
            Logger::crash("Failed to allocate command buffers.");
        }
    }

    void Renderer::freeCommandBuffers() {
        vkFreeCommandBuffers(graphicsDevice_.device(), graphicsDevice_.commandPool(),
            static_cast<uint32_t>(commandBuffers_.size()), commandBuffers_.data());
        commandBuffers_.clear();
    }

    VkCommandBuffer Renderer::beginFrame() {
        if (isFrameStarted_) Logger::crash("Cannot call beginFrame while already in progress.");

        auto result = swapChain_->acquireNextImage(&currentImageIndex_);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            Logger::crash("Failed to acquire swap chain image.");
        }

        isFrameStarted_ = true;

        auto commandBuffer = currentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            Logger::crash("Failed to begin recording command buffers.");
        }
        return commandBuffer;
    }

    void Renderer::endFrame() {
        if (!isFrameStarted_) Logger::crash("Cannot call endFrame while frame is not in progress.");

        auto commandBuffer = currentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            Logger::crash("Failed to record command buffer.");
        }

        auto result = swapChain_->submitCommandBuffers(&commandBuffer, &currentImageIndex_);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window_.wasResized()) {
            window_.resetWindowResizedFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            Logger::crash("Failed to present swap chain image.");
        }

        isFrameStarted_ = false;
        currentFrameIndex_ = (currentFrameIndex_ + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }
    
    void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        if (!isFrameStarted_) Logger::crash("Cannot call beginSwapChainRenderPass while frame is not in progress.");
        if (commandBuffer != currentCommandBuffer()) {
            Logger::crash("Cannot begin render pass on command buffer from a different frame.");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain_->renderPass();
        renderPassInfo.framebuffer = swapChain_->frameBuffer(currentImageIndex_);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain_->extent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain_->extent().width);
        viewport.height = static_cast<float>(swapChain_->extent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, swapChain_->extent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        if (!isFrameStarted_) Logger::crash("Cannot call endSwapChainRenderPass while frame is not in progress.");
        if (commandBuffer != currentCommandBuffer()) {
            Logger::crash("Cannot end render pass on command buffer from a different frame.");
        }

        vkCmdEndRenderPass(commandBuffer);
    }
}
