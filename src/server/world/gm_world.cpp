#include "gm_world.hpp"

#include "../../util/gm_logger.hpp"

#include <functional>
#include <thread>

namespace game {
    World::World(EntityPool& entityPool) : entityPool_{entityPool} {
        
    }
    
    World::~World() {
        // Save & quit
    }

    void World::load(const std::string& world) {
        // Load
    }

    void World::update() {
        std::vector<std::thread> tasks;

        // Update entities
        std::thread aiTask([&] (AIPool& pool) {
            pool.update();
        }, std::ref(aiPool_));
        std::thread physicsTask([&] (PhysicsPool& pool) {
            pool.update();
        }, std::ref(physicsPool_));

        // Update world

        // Wait for threads
        aiTask.join();
        physicsTask.join();
    }

    void World::save() {
        // Save
    }
}
