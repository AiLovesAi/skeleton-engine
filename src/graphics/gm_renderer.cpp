#include "gm_renderer.hpp"

#include "../util/gm_logger.hpp"

#include <array>

namespace game {
     Renderer::Renderer(
        GraphicsInstance* instance,
        GraphicsDevice* device,
        Window* window
    ) : graphicsInstance{instance}, graphicsDevice{device}, window{window} {
        if (instance == nullptr) Logger::crash("Graphics instance passed to Renderer is null.");
        if (device == nullptr) Logger::crash("Graphics device passed to Renderer is null.");
        if (window == nullptr) Logger::crash("Window passed to Renderer is null.");

        recreateSwapChain();
        createCommandBuffers();
    }

    Renderer::~Renderer() {
        freeCommandBuffers();
    }

    void Renderer::recreateSwapChain() {
        auto extent = window->getExtent();
        
        while (extent.width == 0 || extent.height == 0) {
            glfwWaitEvents();
            extent = window->getExtent();
        }

        vkDeviceWaitIdle(graphicsDevice->getDevice());
        
        if (swapChain == nullptr) {
            swapChain = std::make_unique<SwapChain>(graphicsInstance, graphicsDevice, extent);
        } else {
            std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain);
            swapChain = std::make_unique<SwapChain>(graphicsInstance, graphicsDevice, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*swapChain.get())) {
                Logger::crash("Swap chain image or depth format has changed.");
            }
        }
    }

    void Renderer::createCommandBuffers() {
        commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = graphicsDevice->getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(graphicsDevice->getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            Logger::crash("Failed to allocate command buffers.");
        }
    }

    void Renderer::freeCommandBuffers() {
        vkFreeCommandBuffers(graphicsDevice->getDevice(), graphicsDevice->getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    VkCommandBuffer Renderer::beginFrame() {
        if (isFrameStarted) Logger::crash("Cannot call beginFrame while already in progress.");

        auto result = swapChain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            Logger::crash("Failed to acquire swap chain image.");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            Logger::crash("Failed to begin recording command buffers.");
        }
        return commandBuffer;
    }

    void Renderer::endFrame(Camera *camera) {
        if (!isFrameStarted) Logger::crash("Cannot call endFrame while frame is not in progress.");

        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            Logger::crash("Failed to record command buffer.");
        }

        auto result = swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->windowResized()) {
            window->resetWindowResizedFlag();
            if (camera) camera->updatePerspective();
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            Logger::crash("Failed to present swap chain image.");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }
    
    void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        if (!isFrameStarted) Logger::crash("Cannot call beginSwapChainRenderPass while frame is not in progress.");
        if (commandBuffer != getCurrentCommandBuffer()) {
            Logger::crash("Cannot begin render pass on command buffer from a different frame.");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->getRenderPass();
        renderPassInfo.framebuffer = swapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, swapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        if (!isFrameStarted) Logger::crash("Cannot call endSwapChainRenderPass while frame is not in progress.");
        if (commandBuffer != getCurrentCommandBuffer()) {
            Logger::crash("Cannot end render pass on command buffer from a different frame.");
        }

        vkCmdEndRenderPass(commandBuffer);
    }
}
