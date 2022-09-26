#include "gm_ai_component.hpp"

namespace game {
    // AIComponent //
    AIComponent::AIComponent(const Entity entity) : entity_{entity} {

    }

    bool AIComponent::update() {
        return false;
    }

    // AIPool //
    void AIPool::create(const Entity entity) {
        pool_.emplace_back(entity);
        indexMap_[entity] = size_++;
    }

    void AIPool::destroy(const size_t index) {
        Entity entity = pool_[index].entity();
        Entity last = pool_[--size_].entity();
        std::swap(pool_[index], pool_[size_]); // Swap last entity to index
        indexMap_[last] = index; // Update the index of the moved entity
        indexMap_.erase(entity); // Release the index held by the index map
        pool_.erase(pool_.end() - 1); // Free the destroyed component's data from the pool
        entityPool_.destroy(entity); // Kill the entity
    }

    void AIPool::update() {
        for (size_t i = 0; i < pool_.size(); i++) {
            if (pool_[i].update()) destroy(i);
        }
    }
}
