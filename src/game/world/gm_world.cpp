#include "gm_world.hpp"

#include "../../util/gm_logger.hpp"

#include <functional>
#include <thread>

namespace game {
    World::World() {
        
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
        std::thread aiTask([&] (AIComponentPool& pool) {
            pool.updateComponents();
        }, std::ref(aiPool_));
        std::thread physicsTask([&] (PhysicsComponentPool& pool) {
            pool.updateComponents();
        }, std::ref(physicsPool_));

        // Update world

        // Wait for threads
        aiTask.join();
        physicsTask.join();
    }

    void World::save() {
        // Save
    }
    
    PhysicsComponent* World::createPhysicsComponent() {
        PhysicsComponent* newComponent = physicsPool_.createObject();
        if (newComponent == nullptr) Logger::crash("Tried to create a new physics component, but the physics pool was full.");
        return newComponent;
    }

    AIComponent* World::createAIComponent() {
        AIComponent* newComponent = aiPool_.createObject();
        if (newComponent == nullptr) Logger::crash("Tried to create a new AI component, but the AI pool was full.");
        return newComponent;
    }
}
