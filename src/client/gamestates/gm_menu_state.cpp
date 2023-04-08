#include "gm_menu_state.hpp"

#include "gm_client_state.hpp"
#include "gm_host_state.hpp"

namespace game {
    MenuState::MenuState() {
        // Load menu
    }

    MenuState::~MenuState() {
        // Unload menu
    }

    void MenuState::update() {
        // Traverse menu
        Client& client = Client::instance();
        client.setGameState(new HostState("world"));
    }

    void MenuState::render(const float128_t lag) {
        auto& renderer = Client::instance().renderer();
        auto commandBuffer = renderer.beginFrame();
        renderer.beginSwapChainRenderPass(commandBuffer);

        // Render GUI

        renderer.endSwapChainRenderPass(commandBuffer);
        renderer.endFrame();
    }
}
