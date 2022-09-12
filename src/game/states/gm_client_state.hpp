#pragma once

#include "gm_game_state.hpp"
#include "../world/gm_world.hpp"
#include "../../graphics/vulkan/gm_renderer.hpp"
#include "../../graphics/game/gm_camera.hpp"

namespace game {
    class ClientState : public GameState {
        public:
            // Constructors
            virtual ~ClientState() {}

            // Functions
            virtual void load();
            virtual void unload();
            virtual void update();
            virtual void render(const double lag);

        private:
            // Variables
            Window* window_;
            Camera* camera_;

            PhysicsComponentPool physicsPool_;
    };
}
