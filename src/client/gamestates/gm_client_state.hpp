#pragma once

#include "gm_game_state.hpp"
#include "../gm_client.hpp"
#include "../components/gm_client_components.hpp"
#include "../graphics/vulkan/gm_renderer.hpp"
#include "../graphics/game/gm_camera.hpp"

#include <common/headers/float.hpp>
#include <server/entities/gm_entity.hpp>
#include <server/world/gm_world.hpp>

namespace game {
    class ClientState : public GameState {
        public:
            // Constructors
            ClientState();
            ~ClientState();

            // Functions
            virtual void update() override;
            virtual void render(const float128_t lag) override;

        private:
            // Variables
            EntityPool _entityPool;
            ClientComponents _clientComponents{_entityPool};

            World _world{_entityPool};

            Camera camera_{
                _entityPool.create(),
                _world,
                Client::instance().window(),
                45.f,
                std::numeric_limits<float32_t>::min(),
                100.0f
            };
    };
}
