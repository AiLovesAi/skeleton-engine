#include "gm_sound_instance.hpp"

namespace game {
    // SoundInstance //
    SoundInstance::SoundInstance(const Entity entity) : _entity{entity} {

    }

    // SoundPool //
    void SoundPool::create(const Entity entity) {
        _pool.emplace_back(entity);
        _indexMap[entity] = _size++;
    }

    void SoundPool::destroy(const size_t index) {
        Entity entity = _pool[index].entity();
        Entity last = _pool[--_size].entity();
        std::swap(_pool[index], _pool[_size]); // Swap last entity to index
        _indexMap[last] = index; // Update the index of the moved entity
        _indexMap.erase(entity); // Release the index held by the index map
        _pool.erase(_pool.end() - 1); // Free the destroyed instance's data from the pool
        _entityPool.destroy(entity); // Kill the entity
    }
}
