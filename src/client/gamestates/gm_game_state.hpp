#pragma once

#include "../graphics/vulkan/gm_renderer.hpp"

#include <common/headers/float.hpp>
#include <server/world/gm_world.hpp>

namespace game {
    class GameState {
        public:
            // Constructors
            virtual ~GameState() {};
            
            // Functions
            virtual void update() = 0;
            virtual void render(const float128_t lag) = 0;
    };
}
