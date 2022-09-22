#pragma once

#include "gm_render_component.hpp"
#include "../entities/gm_entity.hpp"

namespace game {
    class ClientComponents {
        public:
            // Constructors
            ClientComponents(EntityPool entityPool) : entityPool_{entityPool} {}
        
            // Functions
            void render();
            
            RenderPool& ai() { return render_; }
            
        private:
            // Variables
            EntityPool& entityPool_;
            RenderPool render_{entityPool_, 128};
    };
}