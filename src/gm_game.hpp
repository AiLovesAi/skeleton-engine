#pragma once

#include "graphics/gm_window.hpp"
#include "graphics/gm_graphics_device.hpp"
#include "graphics/gm_graphics_instance.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

namespace game {
    class Game {
        public:
            // Constants
            static constexpr char VERSION[] = "1.0.0";
            static constexpr char TITLE[] = "Game";

            // Constructors
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
            static GraphicsInstance* graphicsInstance;
            static GraphicsDevice* graphicsDevice;
    };
}
