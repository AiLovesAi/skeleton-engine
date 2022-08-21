#include "gm_game.hpp"

namespace game {
    bool Game::running = false;
    bool Game::isServer = false;
    std::unordered_map<std::thread::id, std::string> Game::gameThreads;

    Game::Game(const bool isServer) {
        this->isServer = isServer;
    }

    Game::~Game() {
        //
    }

    void Game::start() {
        // Initialize
    }
}