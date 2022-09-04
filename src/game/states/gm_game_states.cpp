#include "gm_game_states.hpp"

#include "../gm_client.hpp"
#include "../gm_server.hpp"

namespace game {
    void MenuState::load() {
        // Load menu
    }

    void MenuState::update() {
        // Traverse menu
    }

    void MenuState::render() {
        auto renderer = Client::instance().getRenderer();
        auto commandBuffer = renderer->beginFrame();
        renderer->beginSwapChainRenderPass(commandBuffer);

        renderer->endSwapChainRenderPass(commandBuffer);
        renderer->endFrame(nullptr);
    }

    void HostState::load() {

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

    void ClientState::load() {

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
