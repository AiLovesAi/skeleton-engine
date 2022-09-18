#pragma once

#include "gm_game_state.hpp"
#include "../world/gm_world.hpp"
#include "../sides/gm_client.hpp"
#include "../../graphics/vulkan/gm_renderer.hpp"
#include "../../graphics/game/gm_camera.hpp"

#include <string>

namespace game {
    class HostState : public GameState {
        public:
            // Constructors
            HostState(const std::string& world);
            ~HostState();

            // Functions
            virtual void update() override;
            virtual void render(const double lag) override;

        private:
            // Variables
            World world_;

            Camera camera_{
                world_.createPhysicsComponent(),
                Client::instance().window(),
                45.f,
                std::numeric_limits<float>::min(),
                100.0f
            };
    };
}
