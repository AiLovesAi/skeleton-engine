#include "gm_game.hpp"

#include "util/gm_cmd_args.hpp"
#include "util/gm_file.hpp"
#include "util/gm_logger.hpp"
#include "game/gm_client.hpp"
#include "game/gm_server.hpp"

#include <ctime>
#include <random>

namespace game {
    int Game::argc;
    char** Game::argv;
    volatile bool Game::running = false;
    bool Game::isServer = false;

    volatile int Game::threadCount = 0;
    std::unordered_map<std::thread::id, std::string> Game::gameThreads;

    std::string Game::graphicsDeviceName = "NULL";
    std::string Game::OSStr = "NULL";
    std::string Game::CPUStr = "NULL";
    std::string Game::executableDir = "NULL";

    void Game::start(const int argc, char** argv) {
        Game::argc = argc;
        Game::argv = argv;
        init();

        if (isServer) {
            running = true;
            Server server;
            server.start();
        } else {
            Client::start();
        }

        // Wait for threads to stop
        while (threadCount) std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void Game::init() {
        gameThreads.emplace(std::this_thread::get_id(), "Main");
        std::srand(std::time(0));
        
        File::init();
        Logger::init("logs/latest.log", "logs/crash.txt");
        Logger::logMsg(LOG_INFO, "Initializing game.");
        parseArgs();

        Logger::logMsg(LOG_INFO, "Using CPU: " + CPUStr);
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

    void Game::createThread(const std::string& name, void (*function)(void)) {
        std::thread t(function);
        gameThreads.emplace(t.get_id(), name);
        t.detach();
        ++threadCount;
    }
}