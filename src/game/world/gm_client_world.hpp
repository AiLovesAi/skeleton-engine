#pragma once

#include "gm_world.hpp"
#include "../../graphics/gm_camera.hpp"

namespace game {
    class ClientWorld : public World {
        public:
            // Constructors
            ClientWorld();
            ~ClientWorld();

            ClientWorld(const ClientWorld &) = delete;
            ClientWorld &operator=(const ClientWorld &) = delete;
            ClientWorld(ClientWorld&&) = delete;
            ClientWorld &operator=(ClientWorld&&) = delete;

            void updateClient();

        private:
            // Variables
            Camera camera;
    };
}
