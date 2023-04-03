#include "gm_world.hpp"

#include <common/data/file/gm_logger.hpp>

#include <functional>
#include <thread>

namespace game {
    World::World(EntityPool& entityPool) : _entityPool{entityPool} {
        
    }
    
    World::~World() {
        // Save & quit
    }

    void World::load(const std::string& world) {
        // Load
    }

    void World::update() {
        // Update entities
        _serverComponents.update();

        // Update world
        // TODO
    }

    void World::save() {
        // Save
    }
}
