#pragma once

#include "graphics/gm_window.hpp"

#include <unordered_map>
#include <thread>
#include <string>

namespace game {
    class Game {
        public:
            // Constructors
            Game(const int argc, char** argv);
            ~Game();

            // Functions
            void init();
            void start();

            // Variables
            static int argc;
            static char** argv;
            static bool running;
            static bool isServer;
            static std::unordered_map<std::thread::id, std::string> gameThreads;

            static std::string OSStr;
            static std::string CPUStr;
            static std::string executableDir;
            static std::string graphicsDeviceName;
        private:
            // Functions
            void parseArgs();

            // Variables
            Window window{800, 600, "Skeleton Engine"};

    };
}
