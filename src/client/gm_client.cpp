#include "gm_client.hpp"

#include "gamestates/gm_menu_state.hpp"
#include "gamestates/gm_host_state.hpp"
#include "gamestates/gm_client_state.hpp"
#include "graphics/vulkan/gm_swap_chain.hpp"

#include <common/data/file/gm_logger.hpp>

#include <thread>

namespace game {
    Client& Client::instance() {
        static Client *_instance = new Client();
        return *_instance;
    }

    Client::Client() {
        // Initialize sound
        // TODO

        _globalDescriptorPool = DescriptorPool::Builder(_graphicsDevice)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        
        _gameState = new MenuState();
    }

    Client::~Client() {
        // Free sound
        // TODO

        // Free graphics
        glfwTerminate();
        _globalDescriptorPool.reset();
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
        Logger::log(LOG_INFO, "Starting client...");

        // Spawn threads
        //std::thread soundThread(sound, this);

        // Load game

        // Start game
        Core::running = true;
        _window.show();

        // Main game loop
        auto previousTime = std::chrono::high_resolution_clock::now();
        double lag = 0.0f;
        while (Core::running && !_window.shouldClose()) {
            glfwPollEvents();

            auto currentTime = std::chrono::high_resolution_clock::now();
            double elapsedTime = std::chrono::duration<double, std::chrono::milliseconds::period>(currentTime - previousTime).count();
            previousTime = currentTime;
            lag += elapsedTime;

            // Prioritize game update when behind, skip to rendering when ahead
            while (lag >= Core::MS_PER_TICK) {
                if (_nextGameState) {
                    delete _gameState;
                    _gameState = _nextGameState;
                    _nextGameState = nullptr;
                    previousTime = std::chrono::high_resolution_clock::now();
                }

                _gameState->update();
                lag -= Core::MS_PER_TICK;
            }

            // Render object positions between ticks (input is percentage of next tick)
            // Example: Bullet is on left of screen on tick 1, and right on tick two, but render happens
            // at tick 1.5. Input is 0.5, meaning the bullet should render in the middle of the screen.
            _gameState->render(lag / Core::MS_PER_TICK);
        }

        // Unload game
        Core::running = false;
        delete _gameState;

        // Wait for device to stop
        vkDeviceWaitIdle(_graphicsDevice.device());

        // Wait for threads to finish
        //if (soundThread.joinable()) soundThread.join();
    }
}
