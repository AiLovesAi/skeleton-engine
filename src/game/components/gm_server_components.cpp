#include "gm_server_components.hpp"

#include <threads>
#include <vector>

namespace game {
    void ServerComponents::update() {
        std::vector<std::thread> tasks;

        // Update entities
        std::thread aiTask([&] (AIPool& pool) {
            pool.update();
        }, std::ref(ai_));
        std::thread physicsTask([&] (PhysicsPool& pool) {
            pool.update();
        }, std::ref(physics_));

        // Wait for threads
        aiTask.join();
        physicsTask.join();
    }
}
