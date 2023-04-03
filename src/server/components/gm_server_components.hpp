#pragma once

#include "gm_ai_component.hpp"
#include "gm_transform_component.hpp"
#include <server/entities/gm_entity.hpp>

namespace game {
    class ServerComponents{
        public:
            // Constructors
            ServerComponents(EntityPool entityPool) : _entityPool{entityPool} {}
        
            
            // Functions
            void update();
            
            AIPool& ai() { return _ai; }
            TransformPool& transform() { return _transform; }
            
        private:
            // Variables
            EntityPool& _entityPool;
            AIPool _ai{_entityPool, 256};
            TransformPool _transform{_entityPool, 256};
    };
}
