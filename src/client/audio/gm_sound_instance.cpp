#include "gm_sound_instance.hpp"

namespace game {
    // SoundInstance //
    SoundInstance::SoundInstance(const Entity entity) : entity_{entity} {

    }

    // SoundPool //
    void SoundPool::create(const Entity entity) {
        pool_.emplace_back(entity);
        indexMap_[entity] = size_++;
    }

    void SoundPool::destroy(const size_t index) {
        Entity entity = pool_[index].entity();
        Entity last = pool_[--size_].entity();
        std::swap(pool_[index], pool_[size_]); // Swap last entity to index
        indexMap_[last] = index; // Update the index of the moved entity
        indexMap_.erase(entity); // Release the index held by the index map
        pool_.erase(pool_.end() - 1); // Free the destroyed instance's data from the pool
        entityPool_.destroy(entity); // Kill the entity
    }
}
