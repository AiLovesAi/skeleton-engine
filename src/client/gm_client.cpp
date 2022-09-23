#include "gm_client.hpp"

#include "../states/gm_menu_state.hpp"
#include "../states/gm_host_state.hpp"
#include "../states/gm_client_state.hpp"
#include "../../gm_game.hpp"
#include "../../util/gm_logger.hpp"
#include "../../graphics/vulkan/gm_swap_chain.hpp"

#include <thread>

namespace game {
    Client::Client() {
        // Initialize sound
        // TODO

        globalPool_ = DescriptorPool::Builder(graphicsDevice_)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        
        gameState_ = new MenuState();

        Game::running = true;
    }

    Client::~Client() {
        // Free sound
        // TODO

        // Free graphics
        glfwTerminate();
        globalPool_.reset();
    }

    void Client::init() {
        static bool initialized_ = false;

        if (!initialized_) {
            // Initialize graphics
            if (glfwInit() != GLFW_TRUE) {
                Logger::crash("Failed to initialize GLFW.");
            }

            Window::init();

            initialized_ = true;
        }
    }

    void Client::start() {
        // Spawn threads
        //std::thread soundThread(sound, this);

        // Load game

        // Start game
        window_.show();

        // Main game loop
        auto previousTime = std::chrono::high_resolution_clock::now();
        double lag = 0.0f;
        while (Game::running && !window_.shouldClose()) {
            glfwPollEvents();

            auto currentTime = std::chrono::high_resolution_clock::now();
            double elapsedTime = std::chrono::duration<double, std::chrono::milliseconds::period>(currentTime - previousTime).count();
            previousTime = currentTime;
            lag += elapsedTime;

            // Prioritize game update when behind, skip to rendering when ahead
            while (lag >= Game::MS_PER_TICK) {
                gameState_->update();
                lag -= Game::MS_PER_TICK;
            }

            // Render object positions between ticks (input is percentage of next tick)
            // Example: Bullet is on left of screen on tick 1, and right on tick two, but render happens
            // at tick 1.5. Input is 0.5, meaning the bullet should render in the middle of the screen.
            gameState_->render(lag / Game::MS_PER_TICK);

            if (nextGameState_) {
                delete gameState_;
                gameState_ = nextGameState_;
                nextGameState_ = nullptr;
                previousTime = std::chrono::high_resolution_clock::now();
            }
        }

        // Unload game
        Game::running = false;
        delete gameState_;

        // Wait for device to stop
        vkDeviceWaitIdle(graphicsDevice_.device());

        // Wait for threads to finish
        //if (soundThread.joinable()) soundThread.join();
    }
}