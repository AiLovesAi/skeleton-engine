#pragma once

#include "gm_game_object.hpp"
#include "../graphics/gm_graphics_device.hpp"
#include "../graphics/gm_graphics_instance.hpp"
#include "../graphics/gm_window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
            void start();

            // Object variables
            std::map<GameObject::id_t, GameObject*> renderableObjects;
        private:
            GraphicsInstance* graphicsInstance;
            GraphicsDevice* graphicsDevice;
            Window* window;
    };
}