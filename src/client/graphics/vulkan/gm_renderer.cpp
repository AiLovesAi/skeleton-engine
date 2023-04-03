#include "gm_renderer.hpp"

#include <common/data/file/gm_logger.hpp>

#include <array>

namespace game {
     Renderer::Renderer(
        GraphicsInstance& instance,
        GraphicsDevice& device,
        Window& window
    ) : _graphicsInstance{instance}, _graphicsDevice{device}, _window{window} {
        _viewport.maxDepth = 1.0f;

        _clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        _clearValues[1].depthStencil = {1.0f, 0};
        
        _commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        _renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        _renderPassInfo.clearValueCount = static_cast<uint32_t>(_clearValues.size());
        _renderPassInfo.pClearValues = _clearValues.data();

        recreateSwapChain();
        createCommandBuffers();
    }

    Renderer::~Renderer() {
        freeCommandBuffers();
    }

    void Renderer::recreateSwapChain() {
        auto extent = _window.extent();
        
        while (extent.width == 0 || extent.height == 0) {
            glfwWaitEvents();
            extent = _window.extent();
        }

        vkDeviceWaitIdle(_graphicsDevice.device());
        
        if (_swapChain == nullptr) {
            _swapChain = std::make_unique<SwapChain>(_graphicsInstance, _graphicsDevice, extent);
        } else {
            // TODO Edit existing swapchain without setting present mode, etc.
            std::shared_ptr<SwapChain> oldSwapChain = std::move(_swapChain);
            _swapChain = std::make_unique<SwapChain>(_graphicsInstance, _graphicsDevice, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*_swapChain.get())) {
                Logger::crash("Swap chain image or depth format has changed.");
            }
        }
        
        _renderPassInfo.renderPass = _swapChain->renderPass();
        _renderPassInfo.renderArea.extent = _swapChain->extent();
        _viewport.width = static_cast<float>(_swapChain->width());
        _viewport.height = static_cast<float>(_swapChain->height());
        _scissor.extent = _swapChain->extent();
    }

    void Renderer::createCommandBuffers() {
        _commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = _graphicsDevice.commandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

        vkAllocateCommandBuffers(_graphicsDevice.device(), &allocInfo, _commandBuffers.data());
    }

    void Renderer::freeCommandBuffers() {
        vkFreeCommandBuffers(_graphicsDevice.device(), _graphicsDevice.commandPool(),
            static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
        _commandBuffers.clear();
    }

    VkCommandBuffer Renderer::beginFrame() {
        _swapChain->acquireNextImage(&_currentImageIndex);

        auto commandBuffer = currentCommandBuffer();
        vkBeginCommandBuffer(commandBuffer, &_commandBufferInfo);
        return commandBuffer;
    }

    void Renderer::endFrame() {
        auto commandBuffer = currentCommandBuffer();
        vkEndCommandBuffer(commandBuffer);

        _swapChain->submitCommandBuffers(&commandBuffer, &_currentImageIndex);
        if (_window.wasResized()) {
            _window.resetWindowResizedFlag();
            recreateSwapChain();
        }

        _currentFrameIndex = (_currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }
    
    void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        _renderPassInfo.framebuffer = _swapChain->frameBuffer(_currentImageIndex);

        vkCmdBeginRenderPass(commandBuffer, &_renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);        
        vkCmdSetViewport(commandBuffer, 0, 1, &_viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &_scissor);
    }
}
