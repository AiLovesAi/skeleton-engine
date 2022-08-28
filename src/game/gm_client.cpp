#include "gm_client.hpp"

#include "../gm_game.hpp"
#include "../util/gm_logger.hpp"
#include "../graphics/gm_swap_chain.hpp"

#include <thread>

namespace game {
    GraphicsInstance* Client::graphicsInstance = nullptr;
    GraphicsDevice* Client::graphicsDevice = nullptr;
    Window* Client::window = nullptr;

    std::unique_ptr<DescriptorPool> Client::globalPool;

    std::map<GameObject::id_t, GameObject*> Client::renderableObjects;

    Client::Client() {
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

    Client::~Client() {
        // Free graphics
        if (window) delete window;
        glfwTerminate();
        globalPool.reset();
        if (graphicsDevice) delete graphicsDevice;
        if (graphicsInstance) delete graphicsInstance;

        // Free sound
        // TODO
    }

    void Client::start() {
        // Spawn threads
        std::thread renderThread(render);

        // Start game
        window->show();

        while (Game::running && !window->shouldClose()) {
            glfwPollEvents();
        }

        Game::running = false;

        // Wait for threads to finish
        if (renderThread.joinable()) renderThread.join();
    }

    void Client::render() {
        Game::gameThreads.emplace(std::this_thread::get_id(), "Render");
        Logger::logMsg(LOG_INFO, "Render thread started.");

        while (Game::running) {
            
        }

        // Wait for device to stop
        vkDeviceWaitIdle(graphicsDevice->getDevice());

        Game::gameThreads.erase(std::this_thread::get_id());
    }
}
