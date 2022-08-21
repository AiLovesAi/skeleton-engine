#pragma once

#include "graphics/gm_window.hpp"

#include <unordered_map>
#include <thread>
#include <string>

namespace game {
    class Game {
        public:
            Game(const bool isServer);
            ~Game();

            void start();

            static bool running;
            static bool isServer;
            static std::unordered_map<std::thread::id, std::string> gameThreads;
        private:
            Window window{800, 600, "Skeleton Engine"};

    };
}
