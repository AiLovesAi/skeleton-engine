#include "gm_world.hpp"

#include "../../util/gm_logger.hpp"

#include <functional>
#include <thread>
#include <vector>

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
        tasks.push_back(
            std::thread([&] (AIComponentPool& pool) {
                pool.updateComponents();
            }, std::ref(aiPool_))
        );
        tasks.push_back(
            std::thread([&] (PhysicsComponentPool& pool) {
                pool.updateComponents();
            }, std::ref(physicsPool_))
        );

        // Update world

        // Wait for threads
        std::for_each(
            tasks.begin(),
            tasks.end(),
            [](std::thread& t) {
                t.join();
            }
        );
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
