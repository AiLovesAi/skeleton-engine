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
            // Functions
            static void start();
        private:
            // Functions
            static void init();
            static void stop();
            static void render();

            // Variables
            static GraphicsInstance* graphicsInstance;
            static GraphicsDevice* graphicsDevice;
            static Window* window;

            static std::unique_ptr<DescriptorPool> globalPool;
            
            static std::map<GameObject::id_t, GameObject*> renderableObjects;
    };
}