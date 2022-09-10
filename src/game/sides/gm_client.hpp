#pragma once

#include "../objects/gm_game_object.hpp"
#include "../states/gm_game_states.hpp"
#include "../../graphics/gm_graphics_device.hpp"
#include "../../graphics/gm_graphics_instance.hpp"
#include "../../graphics/gm_renderer.hpp"
#include "../../graphics/gm_window.hpp"
#include "../../graphics/gm_descriptors.hpp"

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
            static Client& instance() {
                static Client *instance_ = new Client();
                return *instance_;
            }
            void start();

            GraphicsInstance* graphicsInstance() { return graphicsInstance_; }
            GraphicsDevice* graphicsDevice() { return graphicsDevice_; }
            Renderer* renderer() { return renderer_; }
            Window* window() { return window_; }

            void setGameState(GameState& gameState) { gameState_ = &gameState; }

            // Variables
            static MenuState menuState;
            static HostState hostState;
            static ClientState clientState;

        private:
            // Constructors
            Client();

            // Functions
            void game();
            void render();

            // Variables
            GraphicsInstance* graphicsInstance_ = nullptr;
            GraphicsDevice* graphicsDevice_ = nullptr;
            Renderer* renderer_ = nullptr;
            std::unique_ptr<DescriptorPool> globalPool_;
            Window* window_ = nullptr;

            GameState* gameState_ = &menuState;
    };
}