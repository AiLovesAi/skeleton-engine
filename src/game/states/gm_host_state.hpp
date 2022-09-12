#pragma once

#include "gm_game_state.hpp"
#include "../world/gm_world.hpp"
#include "../../graphics/vulkan/gm_renderer.hpp"
#include "../../graphics/game/gm_camera.hpp"

namespace game {
    class HostState : public GameState {
        public:
            // Constructors
            virtual ~HostState() {}

            // Functions
            virtual void load();
            virtual void unload();
            virtual void update();
            virtual void render(const double lag);

        private:
            // Variables
            Window* window_;
            Camera* camera_;

            World* world_;
    };
}
