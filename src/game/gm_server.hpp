#pragma once

#include "gm_game_object.hpp"

#include <map>

namespace game {
    class Server {
        public:
        // Constructors
            Server();
            ~Server();

            Server(const Server &) = delete;
            Server &operator=(const Server &) = delete;
            Server(Server&&) = delete;
            Server &operator=(Server&&) = delete;

            // Functions

            // Object variables
            std::map<GameObject::id_t, GameObject> gameObjects;
    };
}