#pragma once

#include "gm_game_state.hpp"
#include "../gm_client.hpp"
#include "../components/gm_client_components.hpp"
#include "../graphics/vulkan/gm_renderer.hpp"
#include "../graphics/game/gm_camera.hpp"

#include <common/headers/float.hpp>
#include <server/gm_server.hpp>

#include <string>
#include <vector>

namespace game {
    class HostState : public GameState {
        public:
            // Constructors
            HostState(const std::string& world);
            ~HostState();

            // Functions
            virtual void update() override;
            virtual void render(const float128_t lag) override;

        private:
            // Variables
            Server _server;
            ClientComponents _clientComponents{_server.entityPool()};

            Camera camera_{
                _server.entityPool().create(),
                _server.world(),
                Client::instance().window(),
                45.f,
                std::numeric_limits<float32_t>::min(),
                100.0f
            };
    };
}
