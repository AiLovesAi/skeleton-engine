#pragma once

#include "../../graphics/vulkan/gm_renderer.hpp"
#include "../world/gm_world.hpp"

namespace game {
    class GameState {
        public:
            // Constructors
            virtual ~GameState() {}
            
            // Functions
            virtual void load() {}
            virtual void unload() {}
            virtual void update() {}
            virtual void render(const double lag) {}
    };
}
