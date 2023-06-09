#include "gm_server_components.hpp"

#include <thread>
#include <vector>

namespace game {
    void ServerComponents::update() {
        std::vector<std::thread> tasks;

        // Update entities
        std::thread aiTask([&] (AIPool& pool) {
            pool.update();
        }, std::ref(_ai));
        /*std::thread physicsTask([&] (PhysicsPool& pool) {
            pool.update();
        }, std::ref(physics_));
        */

        // Wait for threads
        aiTask.join();
        //physicsTask.join();
    }
}
