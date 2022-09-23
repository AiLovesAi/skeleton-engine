#pragma once

#include "gm_ai_component.hpp"
#include "gm_transform_component.hpp"
#include "../entities/gm_entity.hpp"

namespace game {
    class ServerComponents{
        public:
            // Constructors
            ServerComponents(EntityPool entityPool) : entityPool_{entityPool} {}
        
            
            // Functions
            void update();
            
            AIPool& ai() { return ai_; }
            TransformPool& transform() { return transform_; }
            
        private:
            // Variables
            EntityPool& entityPool_;
            AIPool ai_{entityPool_, 256};
            TransformPool transform_{entityPool_, 256};
    };
}
