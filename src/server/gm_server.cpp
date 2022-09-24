#include "gm_server.hpp"

namespace game {
    Server& Server::instance() {
        static Server* instance = new Server();
        return *instance;
    }
    
    Server::Server() {}

    Server::~Server() {}

    void Server::start() {}
}
