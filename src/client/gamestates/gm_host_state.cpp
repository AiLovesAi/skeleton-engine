#include "gm_host_state.hpp"

namespace game {
    HostState::HostState(const std::string& world) {
        // Load world and shaders
        _server.world().load(world);
    }

    HostState::~HostState() {
        // Unload world and shaders
    }

    void HostState::update() {
        // Update server
        _server.update();
    }

    void HostState::render(const float128_t lag) {
        camera_.update(lag);

        auto& renderer = Client::instance().renderer();
        auto commandBuffer = renderer.beginFrame();
        renderer.beginSwapChainRenderPass(commandBuffer);

        // Render world
        _clientComponents.render(lag);

        renderer.endSwapChainRenderPass(commandBuffer);
        renderer.endFrame();
    }
}
