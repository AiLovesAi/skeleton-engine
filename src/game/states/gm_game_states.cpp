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

        // Render GUI

        renderer->endSwapChainRenderPass(commandBuffer);
        renderer->endFrame(nullptr);
    }

    void HostState::load() {
        // Load world and shaders
    }

    void HostState::update() {
        // Update world
    }

    void HostState::render() {
        auto renderer = Client::instance().getRenderer();
        auto commandBuffer = renderer->beginFrame();
        renderer->beginSwapChainRenderPass(commandBuffer);

        // Render world

        renderer->endSwapChainRenderPass(commandBuffer);
        renderer->endFrame(nullptr);
    }

    void ClientState::load() {
        // Connect and load shaders
    }

    void ClientState::update() {
        // Listen/send to server
    }

    void ClientState::render() {
        auto renderer = Client::instance().getRenderer();
        auto commandBuffer = renderer->beginFrame();
        renderer->beginSwapChainRenderPass(commandBuffer);

        // Render world

        renderer->endSwapChainRenderPass(commandBuffer);
        renderer->endFrame(nullptr);
    }
}
