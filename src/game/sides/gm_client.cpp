#include "gm_client.hpp"

#include "../../gm_game.hpp"
#include "../../util/gm_logger.hpp"
#include "../../graphics/vulkan/gm_swap_chain.hpp"

#include <thread>

namespace game {
    Client::Client() {
        // Initialize sound
        // TODO

        // Initialize graphics
        if (glfwInit() != GLFW_TRUE) {
            Logger::crash("Failed to initialize GLFW.");
        }

        Window::init();
        
        window_ = new Window(Game::TITLE);
        graphicsInstance_ = new GraphicsInstance(window_);
        graphicsDevice_ = new GraphicsDevice(graphicsInstance_);
        renderer_ = new Renderer(graphicsInstance_, graphicsDevice_, window_);

        globalPool_ = DescriptorPool::Builder(graphicsDevice_)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        Game::running = true;
    }

    Client::~Client() {
        // Free sound
        // TODO

        // Free graphics
        if (renderer_) delete renderer_;
        if (window_) delete window_;
        glfwTerminate();
        globalPool_.reset();
        if (graphicsDevice_) delete graphicsDevice_;
        if (graphicsInstance_) delete graphicsInstance_;
    }

    void Client::start() {
        // Spawn threads
        std::thread gameThread(game, this);

        // Start game
        window_->show();

        // Poll events
        while (Game::running && !window_->shouldClose()) {
            glfwWaitEvents();
        }

        Game::running = false;

        // Wait for threads to finish
        if (gameThread.joinable()) gameThread.join();
    }

    void Client::game() {
        Game::gameThreads.emplace(std::this_thread::get_id(), "Game");
        Logger::logMsg(LOG_INFO, "Game thread started.");
        
        // Load game
        gameState_->load();

        auto previousTime = std::chrono::high_resolution_clock::now();
        double lag = 0.0f;
        while (Game::running) {
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
                gameState_->unload();
                gameState_ = nextGameState_;
                nextGameState_ = nullptr;
                gameState_->load();
                previousTime = std::chrono::high_resolution_clock::now();
            }
        }
        
        // Unload game
        gameState_->unload();

        // Wait for device to stop
        vkDeviceWaitIdle(graphicsDevice_->device());

        Game::gameThreads.erase(std::this_thread::get_id());
    }
}
