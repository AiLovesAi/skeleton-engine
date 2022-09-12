#include "gm_host_state.hpp"

#include "../sides/gm_client.hpp"
#include "../sides/gm_server.hpp"
#include "../../util/gm_logger.hpp"

#include <limits>

namespace game {
    void HostState::load() {
        Client& client = Client::instance();
        window_ = client.window();

        // Load world and shaders
        world_ = new World(client.gameStateArgs());
        camera_ = new Camera(world_->createPhysicsComponent(), window_, 45.f, std::numeric_limits<float>::min(), 100.0f);
    }

    void HostState::unload() {
        // Unload world and shaders
        if (camera_) delete camera_;
        if (world_) delete world_;
    }

    void HostState::update() {
        // Update world
        world_->update();
    }

    void HostState::render(const double lag) {
        camera_->update();

        auto renderer = Client::instance().renderer();
        auto commandBuffer = renderer->beginFrame();
        renderer->beginSwapChainRenderPass(commandBuffer);

        // Render world

        renderer->endSwapChainRenderPass(commandBuffer);
        renderer->endFrame(nullptr);
    }
}
