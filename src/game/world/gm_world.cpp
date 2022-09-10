#include "gm_world.hpp"

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
}
