#pragma once

#include <string>
#include <thread>
#include <unordered_map>

namespace game {
    class Game {
        public:
            // Constants
            static constexpr char VERSION[] = "1.0.0";
            static constexpr char TITLE[] = "Game";

            // Functions
            static void start(const int argc, char** argv);
            static void createThread(const std::string& name, void (*function)(void));

            // Variables
            static volatile bool running;
            static bool isServer;

            static volatile int threadCount;
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
