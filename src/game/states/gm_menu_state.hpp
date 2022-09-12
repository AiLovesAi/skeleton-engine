#pragma once

#include "gm_game_state.hpp"
#include "../../graphics/vulkan/gm_renderer.hpp"

namespace game {
    class MenuState : public GameState {
        public:
            // Constructors
            virtual ~MenuState() {}
            
            // Functions
            virtual void load();
            virtual void unload();
            virtual void update();
            virtual void render(const double lag);

        private:
            // Variables
            // Menu GUI
    };
}
