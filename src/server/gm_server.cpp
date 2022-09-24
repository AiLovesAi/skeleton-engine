#include "gm_server.hpp"

#include <core/gm_core.hpp>

namespace game {
    Server& Server::instance() {
        static Server* instance = new Server();
        return *instance;
    }
    
    Server::Server() {}

    Server::~Server() {}

    void Server::start() {
        Core::running = true;

        while (Core::running) {
            // TODO
        }

        Core::running = false;
    }
}
