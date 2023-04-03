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

            EntityPool& entityPool() { return _entityPool; }
            World& world() { return _world; }
        
        private:
            // Variables
            EntityPool _entityPool;

            World _world{_entityPool};
    };    
}
