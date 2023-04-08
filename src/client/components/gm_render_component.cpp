#include "gm_render_component.hpp"

#include <algorithm>

namespace game {
    // RenderComponent //
    RenderComponent::RenderComponent(const Entity entity) : _entity{entity} {

    }

    void RenderComponent::render(const float128_t lag, const WorldTransform& parentTransform, bool dirty) {

    }

    // RenderPool //
    void RenderPool::create(const Entity entity) {
        _pool.emplace_back(entity);
        _indexMap[entity] = _size++;
    }

    void RenderPool::destroy(const size_t index) {
        Entity entity = _pool[index].entity();
        Entity last = _pool[--_size].entity();
        std::swap(_pool[index], _pool[_size]); // Swap last entity to index
        _indexMap[last] = index; // Update the index of the moved entity
        _indexMap.erase(entity); // Release the index held by the index map
        _pool.erase(_pool.end() - 1); // Free the destroyed component's data from the pool
        _entityPool.destroy(entity); // Kill the entity
    }

    void RenderPool::render(const float128_t lag) {
        for (size_t i = 0; i < _pool.size(); i++) {
            // if (_pool[i].parent == nullptr)
            _pool[i].render(lag, PhysicsComponent::origin, false);
            // TODO Send dirty components to the end of the pool ([clean] [dirty parents] [dirty children])
            // http://bitsquid.blogspot.com/2014/10/building-data-oriented-entity-system.html
        }
    }
}
