#pragma once

#include "objects/gm_game_object.hpp"

#include <map>

namespace game {
    class Client {
        public:
            // Constructors
            Client();
            ~Client();

            Client(const Client &) = delete;
            Client &operator=(const Client &) = delete;
            Client(Client&&) = delete;
            Client &operator=(Client&&) = delete;

            // Functions

            // Object variables
            std::map<GameObject::id_t, GameObject*> renderableObjects;
    };
}