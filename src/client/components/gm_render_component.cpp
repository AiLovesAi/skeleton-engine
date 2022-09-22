#include "gm_render_component.hpp"

#include <algorithm>

namespace game {
    // RenderComponent //
    RenderComponent::RenderComponent(const Entity entity) : entity_{entity} {

    }

    void RenderComponent::render(const WorldTransform& parentTransform, bool dirty) {

    }

    // RenderPool //
    void RenderPool::create(RenderComponent& component) {
        pool_.push_back(component);
        indexMap_[component.entity()] = size_++;
    }

    void RenderPool::destroy(const size_t index) {
        Entity entity = pool_[index].entity();
        Entity last = pool_[--size_].entity();
        std::swap(pool_[index], pool_[size_]); // Swap last entity to index
        indexMap_[last] = index; // Update the index of the moved entity
        indexMap_.erase(entity); // Release the index held by the index map
        pool_.erase(pool_.end() - 1); // Free the destroyed component's data from the pool
        entityPool_.destroy(entity); // Kill the entity
    }

    void RenderPool::render() {
        for (size_t i = 0; i < pool_.size(); i++) {
            // if (pool_[i].parent == nullptr)
            pool_[i].render(PhysicsComponent::origin, false);
            // TODO Send dirty components to the end of the pool ([clean] [dirty parents] [dirty children])
            // http://bitsquid.blogspot.com/2014/10/building-data-oriented-entity-system.html
        }
    }
}
