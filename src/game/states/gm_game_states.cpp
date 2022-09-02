#include "gm_game_states.hpp"

#include "../gm_client.hpp"

namespace game {
    void MenuState::update() {
        
    }

    void MenuState::render() {
        auto renderer = Client::instance().getRenderer();
        auto commandBuffer = renderer->beginFrame();
        renderer->beginSwapChainRenderPass(commandBuffer);

        renderer->endSwapChainRenderPass(commandBuffer);
        renderer->endFrame(nullptr);
    }

    void HostState::update() {

    }

    void HostState::render() {
        auto renderer = Client::instance().getRenderer();
        auto commandBuffer = renderer->beginFrame();
        renderer->beginSwapChainRenderPass(commandBuffer);

        renderer->endSwapChainRenderPass(commandBuffer);
        renderer->endFrame(nullptr);
    }

    void ClientState::update() {

    }

    void ClientState::render() {
        auto renderer = Client::instance().getRenderer();
        auto commandBuffer = renderer->beginFrame();
        renderer->beginSwapChainRenderPass(commandBuffer);

        renderer->endSwapChainRenderPass(commandBuffer);
        renderer->endFrame(nullptr);
    }
}
