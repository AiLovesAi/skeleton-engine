#pragma once

#include "gm_render_component.hpp"

#include <common/headers/float.hpp>
#include <server/entities/gm_entity.hpp>

namespace game {
    class ClientComponents {
        public:
            // Constructors
            ClientComponents(EntityPool entityPool) : _entityPool{entityPool} {}
        
            // Functions
            void render(const float128_t lag);
            
            RenderPool& ai() { return render_; }
            
        private:
            // Variables
            EntityPool& _entityPool;
            RenderPool render_{_entityPool, 128};
    };
}