#include "gm_physics_component.hpp"

namespace game {
    // PhysicsComponent //
    PhysicsComponent::PhysicsComponent(const Entity entity) : _entity{entity} {

    }

    bool PhysicsComponent::update() {
        return false;
    }

    // PhysicsPool //
    void PhysicsPool::create(const Entity entity) {
        _pool.emplace_back(entity);
        _indexMap[entity] = _size++;
    }

    void PhysicsPool::destroy(const size_t index) {
        Entity entity = _pool[index].entity();
        Entity last = _pool[--_size].entity();
        std::swap(_pool[index], _pool[_size]); // Swap last entity to index
        _indexMap[last] = index; // Update the index of the moved entity
        _indexMap.erase(entity); // Release the index held by the index map
        _pool.erase(_pool.end() - 1); // Free the destroyed component's data from the pool
        _entityPool.destroy(entity); // Kill the entity
    }

    void PhysicsPool::update() {
        for (size_t i = 0; i < _pool.size(); i++) {
            if (_pool[i].update()) destroy(i);
        }
    }
}
