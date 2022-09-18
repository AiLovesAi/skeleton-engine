#include "gm_game.hpp"

#include "util/gm_cmd_args.hpp"
#include "util/gm_file.hpp"
#include "util/gm_logger.hpp"
#include "game/sides/gm_client.hpp"
#include "game/sides/gm_server.hpp"

#include <ctime>
#include <random>

namespace game {
    int Game::argc_;
    char** Game::argv_;
    std::atomic<bool> Game::running = false;
    bool Game::isServer = false;

    std::unordered_map<std::thread::id, std::string> Game::gameThreads;

    std::string Game::graphicsDeviceName = "NULL";
    std::string Game::OSStr = "NULL";
    std::string Game::CPUStr = "NULL";
    std::string Game::executableDir = "NULL";

    void Game::start(const int argc, char** argv) {
        Game::argc_ = argc;
        Game::argv_ = argv;
        init();

        if (isServer) {
            running = true;

            Server& server = Server::instance();
            server.start();
        } else {
            Client::init();
            Client& client = Client::instance();
            client.start();
        }
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
        if (CmdArgs::cmdOptionExists(argv_, argv_ + argc_, "--server")) {
            isServer = true;
            Logger::logMsg(LOG_INFO, "Running game as server.");
        } else {
            Logger::logMsg(LOG_INFO, "Running game as client.");
        }
    }
}