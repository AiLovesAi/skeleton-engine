#include "gm_world.hpp"

#include "../../util/gm_logger.hpp"

namespace game {
    World::World(const std::string& name) {
        // Load
    }
    
    World::~World() {
        // Save & quit
    }

    void World::update() {
        // Update entities
        aiPool_.updateComponents();
        physicsPool_.updateComponents();
        
        // Update world
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
