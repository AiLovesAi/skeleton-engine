#include "gm_client.hpp"

#include "../gm_game.hpp"
#include "../util/gm_logger.hpp"

namespace game {
    Client::Client() {
        if (glfwInit() != GLFW_TRUE) {
            Logger::crash("Failed to initialize GLFW.");
        }

        Window::init();
        // Initialize sound
    }

    Client::~Client() {
        if (window) delete window;
        glfwTerminate();

        if (graphicsDevice) delete graphicsDevice;
        if (graphicsInstance) delete graphicsInstance;
    }

    void Client::start() {
        window = new Window(Game::TITLE);
        graphicsInstance = new GraphicsInstance(window);
        graphicsDevice = new GraphicsDevice(graphicsInstance);

        window->show();
        Game::running = true;

        while (Game::running && !window->shouldClose()) {
            glfwPollEvents();
        }

        Game::running = false;

        // Wait for device to stop
        vkDeviceWaitIdle(graphicsDevice->getDevice());
    }
}
