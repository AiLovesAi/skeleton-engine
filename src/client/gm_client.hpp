#pragma once

#include "gamestates/gm_game_state.hpp"
#include "graphics/vulkan/gm_graphics_device.hpp"
#include "graphics/vulkan/gm_graphics_instance.hpp"
#include "graphics/vulkan/gm_renderer.hpp"
#include "graphics/window/gm_window.hpp"
#include "graphics/vulkan/gm_descriptors.hpp"

#include <common/gm_core.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <map>
#include <memory>

namespace game {
    class Client {
        public:
            // Constructors
            ~Client();

            Client(const Client &) = delete;
            Client &operator=(const Client &) = delete;
            Client(Client&&) = default;
            Client &operator=(Client&&) = default;

            // Functions
            static Client& instance();
            static void init();
            void start();

            GraphicsInstance& graphicsInstance() { return graphicsInstance_; }
            GraphicsDevice& graphicsDevice() { return graphicsDevice_; }
            Renderer& renderer() { return renderer_; }
            Window& window() { return window_; }

            void setGameState(GameState*const gameState) { nextGameState_ = gameState; }

        private:
            // Constructors
            Client();

            // Variables
            Window window_{Core::TITLE};
            GraphicsInstance graphicsInstance_{window_};
            GraphicsDevice graphicsDevice_{graphicsInstance_};
            Renderer renderer_{graphicsInstance_, graphicsDevice_, window_};
            std::unique_ptr<DescriptorPool> globalPool_;

            GameState* gameState_ = nullptr;
            GameState* nextGameState_ = nullptr;
    };
}