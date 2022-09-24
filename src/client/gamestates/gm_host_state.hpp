#pragma once

#include "gm_game_state.hpp"
#include "../gm_client.hpp"
#include "../components/gm_client_components.hpp"
#include "../graphics/vulkan/gm_renderer.hpp"
#include "../graphics/game/gm_camera.hpp"

#include <server/world/gm_world.hpp>

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
            virtual void render(const double lag) override;

            World& world() { return world_; }

        private:
            // Variables
            EntityPool entityPool_;
            ClientComponents clientComponents_{entityPool_};

            World world_{entityPool_};

            Camera camera_{
                entityPool_.create(),
                world_,
                Client::instance().window(),
                45.f,
                std::numeric_limits<float>::min(),
                100.0f
            };
    };
}
