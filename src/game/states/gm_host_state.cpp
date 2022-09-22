#include "gm_host_state.hpp"

#include "../sides/gm_server.hpp"

#include <limits>

namespace game {
    HostState::HostState(const std::string& world) {
        // Load world and shaders
        world_.load(world);
    }

    HostState::~HostState() {
        // Unload world and shaders
    }

    void HostState::update() {
        // Update world
        world_.update();
    }

    void HostState::render(const double lag) {
        camera_.update();

        auto& renderer = Client::instance().renderer();
        auto commandBuffer = renderer.beginFrame();
        renderer.beginSwapChainRenderPass(commandBuffer);

        // Render world
        renderPool_.render();

        renderer.endSwapChainRenderPass(commandBuffer);
        renderer.endFrame();
    }
}
