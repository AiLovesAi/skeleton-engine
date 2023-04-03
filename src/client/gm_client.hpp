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

            GraphicsInstance& graphicsInstance() { return _graphicsInstance; }
            GraphicsDevice& graphicsDevice() { return _graphicsDevice; }
            Renderer& renderer() { return renderer_; }
            Window& window() { return _window; }

            void setGameState(GameState*const gameState) { _nextGameState = gameState; }

        private:
            // Constructors
            Client();

            // Variables
            Window _window{Core::TITLE};
            GraphicsInstance _graphicsInstance{_window};
            GraphicsDevice _graphicsDevice{_graphicsInstance};
            Renderer renderer_{_graphicsInstance, _graphicsDevice, _window};
            std::unique_ptr<DescriptorPool> _globalDescriptorPool;

            GameState* _gameState = nullptr;
            GameState* _nextGameState = nullptr;
    };
}