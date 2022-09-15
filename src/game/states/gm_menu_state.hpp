#pragma once

#include "gm_game_state.hpp"
#include "../../graphics/vulkan/gm_renderer.hpp"

namespace game {
    class MenuState : public GameState {
        public:
            // Constructors
            virtual ~MenuState() override {}
            
            // Functions
            virtual void load() override;
            virtual void unload() override;
            virtual void update() override;
            virtual void render(const double lag) override;

        private:
            // Variables
            // Menu GUI
    };
}
