#pragma once

#include <atomic>
#include <string>
#include <thread>
#include <unordered_map>

namespace game {
    class Game {
        public:
            // Constants
            static constexpr char VERSION[] = "1.0.0";
            static constexpr char TITLE[] = "Game";

            static constexpr int MS_PER_TICK = 1000 / 20; // 20 TPS

            // Functions
            static void start(const int argc, char** argv);

            // Variables
            static std::atomic<bool> running;
            static bool isServer;

            static std::unordered_map<std::thread::id, std::string> gameThreads;

            static std::string OSStr;
            static std::string CPUStr;
            static std::string executableDir;
            static std::string graphicsDeviceName;
        private:
            // Functions
            static void init();
            static void parseArgs();

            // Variables 
            static int argc;
            static char** argv;
    };
}
