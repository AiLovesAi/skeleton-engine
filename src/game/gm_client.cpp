#include "gm_client.hpp"

#include "../gm_game.hpp"
#include "../util/gm_logger.hpp"
#include "../graphics/gm_swap_chain.hpp"

#include <thread>

namespace game {
    MenuState Client::menuState;
    HostState Client::hostState;
    ClientState Client::clientState;

    Client::Client() {
        // Initialize graphics
        if (glfwInit() != GLFW_TRUE) {
            Logger::crash("Failed to initialize GLFW.");
        }

        Window::init();
        
        window = new Window(Game::TITLE);
        graphicsInstance = new GraphicsInstance(window);
        graphicsDevice = new GraphicsDevice(graphicsInstance);
        renderer = new Renderer(graphicsInstance, graphicsDevice, window);

        globalPool = DescriptorPool::Builder(graphicsDevice)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        // Initialize sound
        // TODO

        Game::running = true;
    }

    Client::~Client() {
        // Free graphics
        if (renderer) delete renderer;
        if (window) delete window;
        glfwTerminate();
        globalPool.reset();
        if (graphicsDevice) delete graphicsDevice;
        if (graphicsInstance) delete graphicsInstance;

        // Free sound
        // TODO
    }

    void Client::start() {
        // Load menu
        gameState->load();

        // Spawn threads
        std::thread gameThread(game, this);

        // Start game
        window->show();

        // Poll events
        while (Game::running && !window->shouldClose()) {
            glfwWaitEvents();
        }

        Game::running = false;

        // Wait for threads to finish
        if (gameThread.joinable()) gameThread.join();
    }

    void Client::game() {
        Game::gameThreads.emplace(std::this_thread::get_id(), "Game");
        Logger::logMsg(LOG_INFO, "Game thread started.");

        auto previousTime = std::chrono::high_resolution_clock::now();
        double lag = 0.0f;
        while (Game::running) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            double elapsedTime = std::chrono::duration<double, std::chrono::milliseconds::period>(currentTime - previousTime).count();
            previousTime = currentTime;
            lag += elapsedTime;

            // Prioritize game update when behind, skip to rendering when ahead
            while (lag >= Game::MS_PER_TICK) {
                gameState->update();
                lag -= Game::MS_PER_TICK;
            }

            // Render object positions between ticks (input is percentage of next tick)
            // Example: Bullet is on left of screen on tick 1, and right on tick two, but render happens
            // at tick 1.5. Input is 0.5, meaning the bullet should render in the middle of the screen.
            gameState->render(lag / Game::MS_PER_TICK);
        }

        // Wait for device to stop
        vkDeviceWaitIdle(graphicsDevice->getDevice());

        Game::gameThreads.erase(std::this_thread::get_id());
    }
}
