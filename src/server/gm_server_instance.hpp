#pragma once

#include "gm_server.hpp"

#include <map>

namespace game {
    class ServerInstance {
        public:
            // Constructors
            ServerInstance(const ServerInstance &) = delete;
            ServerInstance &operator=(const ServerInstance &) = delete;
            ServerInstance(ServerInstance&&) = default;
            ServerInstance &operator=(ServerInstance&&) = default;

            // Functions
            static ServerInstance& instance();
            void start();
        
        private:
            // Constructors
            ServerInstance();
            ~ServerInstance();

            // Variables
            Server _server;
    };
}