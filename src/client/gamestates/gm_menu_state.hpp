#pragma once

#include "gm_game_state.hpp"
#include "../gm_client.hpp"
#include "../graphics/vulkan/gm_renderer.hpp"

#include <common/headers/float.hpp>

namespace game {
    class MenuState : public GameState {
        public:
            // Constructors
            MenuState();
            ~MenuState();
            
            // Functions
            virtual void update() override;
            virtual void render(const float128_t lag) override;

        private:
            // Variables
            // Menu GUI
            // TODO
    };
}
