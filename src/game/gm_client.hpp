#pragma once

#include "gm_game_object.hpp"
#include "../graphics/gm_graphics_device.hpp"
#include "../graphics/gm_graphics_instance.hpp"
#include "../graphics/gm_window.hpp"
#include "../graphics/gm_descriptors.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <map>
#include <memory>

namespace game {
    class Client {
        public:
            // Constructors
            Client(const Client &) = delete;
            Client &operator=(const Client &) = delete;
            Client(Client&&) = default;
            Client &operator=(Client&&) = default;

            // Functions
            static Client& instance() {
                static Client *instance = new Client();
                return *instance;
            }
            void start();

        private:
            // Constructors
            Client();
            ~Client();

            // Functions
            static void render();

            // Variables
            static GraphicsInstance* graphicsInstance;
            static GraphicsDevice* graphicsDevice;
            static Window* window;

            static std::unique_ptr<DescriptorPool> globalPool;
            
            static std::map<GameObject::id_t, GameObject*> renderableObjects;
    };
}