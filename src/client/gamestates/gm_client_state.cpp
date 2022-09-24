#include "gm_client_state.hpp"

#include <limits>

namespace game {
    ClientState::ClientState() {
        // Connect and load shaders
    }

    ClientState::~ClientState() {
        // Disconnect and unload shaders
    }

    void ClientState::update() {
        // Listen/send to server
    }

    void ClientState::render(const double lag) {
        camera_.update(lag);

        auto& renderer = Client::instance().renderer();
        auto commandBuffer = renderer.beginFrame();
        renderer.beginSwapChainRenderPass(commandBuffer);

        // Render world
        clientComponents_.render(lag);

        renderer.endSwapChainRenderPass(commandBuffer);
        renderer.endFrame();
    }
}
