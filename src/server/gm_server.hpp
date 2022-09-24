#pragma once

#include "entities/gm_entity.hpp"
#include "world/gm_world.hpp"

#include <map>

namespace game {
    class Server {
        public:
            // Constructors
            Server(const Server &) = delete;
            Server &operator=(const Server &) = delete;
            Server(Server&&) = default;
            Server &operator=(Server&&) = default;

            // Functions
            static Server& instance();
            void start();
        
        private:
            // Constructors
            Server();
            ~Server();

            // Variables
            EntityPool entityPool_;

            World world_{entityPool_};
    };
}