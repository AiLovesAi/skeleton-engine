#include "gm_server_instance.hpp"

#include <core/gm_core.hpp>
#include <core/logger/gm_logger.hpp>
#include <core/system/gm_threads.hpp>

namespace game {
    ServerInstance& ServerInstance::instance() {
        static ServerInstance* instance = new ServerInstance();
        return *instance;
    }
    
    ServerInstance::ServerInstance() {}

    ServerInstance::~ServerInstance() {}

    void ServerInstance::start() {
        Logger::log(LOG_INFO, "Starting server...");
        Core::running = true;
        
        // Load game
        // TODO Load world

        // Start server
        // TODO Open on port

        // Main game loop
        auto previousTime = std::chrono::high_resolution_clock::now();
        double lag = 0.0f;
        while (Core::running) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            double elapsedTime = std::chrono::duration<double, std::chrono::milliseconds::period>(currentTime - previousTime).count();
            previousTime = currentTime;
            lag += elapsedTime;

            // Prioritize game update when behind, skip to rendering when ahead
            while (lag >= Core::MS_PER_TICK) {
                world_.update();
                lag -= Core::MS_PER_TICK;
            }

            // Send data
            // TODO Send/receive data
        }

        Core::running = false;
    }
}
