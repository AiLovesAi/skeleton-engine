#include "gm_menu_state.hpp"

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
        client.setGameState(client.hostState());
    }

    void MenuState::render(const double lag) {
        auto renderer = Client::instance().renderer();
        auto commandBuffer = renderer->beginFrame();
        renderer->beginSwapChainRenderPass(commandBuffer);

        // Render GUI

        renderer->endSwapChainRenderPass(commandBuffer);
        renderer->endFrame(nullptr);
    }
}
