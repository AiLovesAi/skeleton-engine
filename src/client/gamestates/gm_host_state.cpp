#include "gm_host_state.hpp"

namespace game {
    HostState::HostState(const std::string& world) {
        // Load world and shaders
        server_.world().load(world);
    }

    HostState::~HostState() {
        // Unload world and shaders
    }

    void HostState::update() {
        // Update server
        server_.update();
    }

    void HostState::render(const double lag) {
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
