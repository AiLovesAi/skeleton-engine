#include "gm_game.hpp"

#include "util/gm_cmd_args.hpp"
#include "util/gm_file.hpp"
#include "util/gm_logger.hpp"

#include <ctime>
#include <random>

namespace game {
    int Game::argc;
    char** Game::argv;
    bool Game::running = false;
    bool Game::isServer = false;
    std::unordered_map<std::thread::id, std::string> Game::gameThreads;
    std::string Game::graphicsDeviceName = "NULL";
    std::string Game::OSStr = "NULL";
    std::string Game::CPUStr = "NULL";
    std::string Game::executableDir = "NULL";

    Window* Game::window = nullptr;

    Game::~Game() {
        delete window;
        glfwTerminate();
    }

    void Game::init(const int argc, char** argv) {
        gameThreads.emplace(std::this_thread::get_id(), "Main");
        std::srand(std::time(0));
        
        File::init();
        Logger::init("logs/latest.log", "logs/crash.txt");
        Logger::logMsg(LOG_INFO, "Initializing game.");
        Window::init();

        Logger::logMsg(LOG_INFO, "Using CPU: " + CPUStr);

        parseArgs();

        if (glfwInit() != GLFW_TRUE) {
            Logger::crash("Failed to initialize GLFW.");
        }
    }

    void Game::parseArgs() {
        // Check if server
        if (CmdArgs::cmdOptionExists(argv, argv + argc, "--server")) {
            isServer = true;
            Logger::logMsg(LOG_INFO, "Running game as server.");
        } else {
            Logger::logMsg(LOG_INFO, "Running game as client.");
        }
    }

    void Game::start() {
        if (!isServer) {
            window = new Window(800, 600, "Game");
            window->show();

            running = true;

            while (running && !window->shouldClose()) {
                glfwPollEvents();
            }

            running = false;

            // Wait for device to stop
            //vkDeviceWaitIdle(graphicsDevice.device());
        }
    }
}