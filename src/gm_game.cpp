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
    std::string graphicsDeviceName = "NULL";
    std::string OSStr = "NULL";
    std::string CPUStr = "NULL";
    std::string executableDir = "NULL";

    Game::Game(const int argc, char** argv) {
        Game::argc = argc;
        Game::argv = argv;
    }

    Game::~Game() {}

    void Game::init() {
        gameThreads.emplace(std::this_thread::get_id(), "Main");
        std::srand(std::time(0));
        
        File::init();
        Logger::init("logs/latest.log", "logs/crash.txt");
        Logger::logMsg(LOG_INFO, "Initializing game.");
        Logger::logMsg(LOG_INFO, "Using CPU: " + CPUStr);

        parseArgs();
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
        // Initialize
    }
}