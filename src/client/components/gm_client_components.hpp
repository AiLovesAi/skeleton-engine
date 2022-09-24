#pragma once

#include "gm_render_component.hpp"
#include <server/entities/gm_entity.hpp>

namespace game {
    class ClientComponents {
        public:
            // Constructors
            ClientComponents(EntityPool entityPool) : entityPool_{entityPool} {}
        
            // Functions
            void render(const double lag);
            
            RenderPool& ai() { return render_; }
            
        private:
            // Variables
            EntityPool& entityPool_;
            RenderPool render_{entityPool_, 128};
    };
}