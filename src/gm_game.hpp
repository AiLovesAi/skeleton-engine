#pragma once

#include "graphics/gm_window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

namespace game {
    class Game {
        public:
            // Constructors
            Game() {}
            ~Game();

            // Functions
            static void init(const int argc, char** argv);
            static void start();

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
            static void parseArgs();

            // Variables
            static Window* window;
    };
}
