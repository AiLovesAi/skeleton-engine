#pragma once

#include "../objects/gm_game_object.hpp"
#include "../states/gm_menu_state.hpp"
#include "../states/gm_host_state.hpp"
#include "../states/gm_client_state.hpp"
#include "../../graphics/vulkan/gm_graphics_device.hpp"
#include "../../graphics/vulkan/gm_graphics_instance.hpp"
#include "../../graphics/vulkan/gm_renderer.hpp"
#include "../../graphics/window/gm_window.hpp"
#include "../../graphics/vulkan/gm_descriptors.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <map>
#include <memory>
#include <string>

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

            GraphicsInstance* graphicsInstance() const { return graphicsInstance_; }
            GraphicsDevice* graphicsDevice() const { return graphicsDevice_; }
            Renderer* renderer() const { return renderer_; }
            Window* window() const { return window_; }

            GameState* menuState() { return &menuState_; }
            GameState* hostState() { return &hostState_; }
            GameState* clientState() { return &clientState_; }
            std::string gameStateArgs() const { return gameStateArgs_; }

            void setGameState(GameState*const gameState) { nextGameState_ = gameState; }

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

            MenuState menuState_;
            HostState hostState_;
            ClientState clientState_;

            GameState* gameState_ = &menuState_;
            GameState* nextGameState_ = nullptr;
            std::string gameStateArgs_ = "NULL";
    };
}