#include "gm_client.hpp"

#include "../gm_game.hpp"
#include "../util/gm_logger.hpp"
#include "../graphics/gm_swap_chain.hpp"

namespace game {
    GraphicsInstance* Client::graphicsInstance = nullptr;
    GraphicsDevice* Client::graphicsDevice = nullptr;
    Window* Client::window = nullptr;

    std::unique_ptr<DescriptorPool> Client::globalPool;

    std::map<GameObject::id_t, GameObject*> Client::renderableObjects;

    void Client::start() {
        init();

        // Spawn threads
        Game::createThread("Render", render);

        // Start game
        window->show();

        while (Game::running && !window->shouldClose()) {
            glfwPollEvents();
        }

        stop();
    }

    void Client::init() {
        // Initialize graphics
        if (glfwInit() != GLFW_TRUE) {
            Logger::crash("Failed to initialize GLFW.");
        }

        Window::init();
        
        window = new Window(Game::TITLE);
        graphicsInstance = new GraphicsInstance(window);
        graphicsDevice = new GraphicsDevice(graphicsInstance);

        globalPool = DescriptorPool::Builder(graphicsDevice)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        // Initialize sound
        // TODO

        Game::running = true;
    }

    void Client::stop() {
        Game::running = false;

        // Wait for device to stop
        vkDeviceWaitIdle(graphicsDevice->getDevice());

        // Wait for threads to finish
        while (Game::threadCount) std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Free graphics
        if (window) delete window;
        glfwTerminate();
        globalPool.reset();
        if (graphicsDevice) delete graphicsDevice;
        if (graphicsInstance) delete graphicsInstance;

        // Free sound
        // TODO
    }

    void Client::render() {
        Logger::logMsg(LOG_INFO, "Render thread started.");

        while (Game::running) {
            
        }

        --Game::threadCount;
    }
}
