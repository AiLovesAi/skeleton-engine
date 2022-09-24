#pragma once

#include "entities/gm_entity.hpp"
#include "world/gm_world.hpp"

namespace game {
    class Server {
        public:
            // Constructors
            Server();
            ~Server();

            // Functions
            void host();
            void update();

            EntityPool& entityPool() { return entityPool_; }
            World& world() { return world_; }
        
        private:
            // Variables
            EntityPool entityPool_;

            World world_{entityPool_};
    };    
}
