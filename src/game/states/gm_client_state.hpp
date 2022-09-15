#pragma once

#include "gm_game_state.hpp"
#include "../world/gm_world.hpp"
#include "../../graphics/vulkan/gm_renderer.hpp"
#include "../../graphics/game/gm_camera.hpp"

namespace game {
    class ClientState : public GameState {
        public:
            // Constructors
            virtual ~ClientState() override {}

            // Functions
            virtual void load() override;
            virtual void unload() override;
            virtual void update() override;
            virtual void render(const double lag) override;

        private:
            // Variables
            Window* window_;
            Camera* camera_;

            PhysicsComponentPool physicsPool_;
    };
}
