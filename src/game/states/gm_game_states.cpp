#include "gm_game_states.hpp"

#include "../sides/gm_client.hpp"
#include "../sides/gm_server.hpp"

namespace game {
    void MenuState::load() {
        // Load menu
    }

    void MenuState::unload() {
        // Unload menu
    }

    void MenuState::update() {
        // Traverse menu
        Client& client = Client::instance();
        client.setGameState(Client::hostState);
    }

    void MenuState::render(const double lag) {
        auto renderer = Client::instance().renderer();
        auto commandBuffer = renderer->beginFrame();
        renderer->beginSwapChainRenderPass(commandBuffer);

        // Render GUI

        renderer->endSwapChainRenderPass(commandBuffer);
        renderer->endFrame(nullptr);
    }

    void HostState::load(std::string& worldName) {
        // Load world and shaders
        world_ = new World(worldName);
    }

    void HostState::unload() {
        // Unload world and shaders
        if (world_) delete world_;
    }

    void HostState::update() {
        // Update world
        world_->update();
    }

    void HostState::render(const double lag) {
        auto renderer = Client::instance().renderer();
        auto commandBuffer = renderer->beginFrame();
        renderer->beginSwapChainRenderPass(commandBuffer);

        // Render world

        renderer->endSwapChainRenderPass(commandBuffer);
        renderer->endFrame(nullptr);
    }

    void ClientState::load() {
        // Connect and load shaders
    }

    void ClientState::unload() {
        // Disconnect and unload shaders
    }

    void ClientState::update() {
        // Listen/send to server
    }

    void ClientState::render(const double lag) {
        auto renderer = Client::instance().renderer();
        auto commandBuffer = renderer->beginFrame();
        renderer->beginSwapChainRenderPass(commandBuffer);

        // Render world

        renderer->endSwapChainRenderPass(commandBuffer);
        renderer->endFrame(nullptr);
    }
}
