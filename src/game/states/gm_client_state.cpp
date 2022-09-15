#include "gm_client_state.hpp"

#include "../sides/gm_client.hpp"
#include "../sides/gm_server.hpp"

#include <limits>

namespace game {
    void ClientState::load() {
        Client& client = Client::instance();
        window_ = client.window();

        // Connect and load shaders
        camera_ = new Camera(physicsPool_.createObject(), window_, 45.f, std::numeric_limits<float>::min(), 100.0f);
    }

    void ClientState::unload() {
        // Disconnect and unload shaders
        if (camera_) delete camera_;
    }

    void ClientState::update() {
        // Listen/send to server
        physicsPool_.updateComponents();
    }

    void ClientState::render(const double lag) {
        camera_->update();

        auto renderer = Client::instance().renderer();
        auto commandBuffer = renderer->beginFrame();
        renderer->beginSwapChainRenderPass(commandBuffer);

        // Render world

        renderer->endSwapChainRenderPass(commandBuffer);
        renderer->endFrame();
    }
}
