#include "gm_sound_instance.hpp"

namespace game {
    // SoundInstancePool //
    SoundInstance* SoundInstancePool::createObject() {
        // If there is no room in the pool, the caller will have to either
        // create a new one or have nothing created.
        if (numSounds_ >= POOL_SIZE) return nullptr;

        SoundInstance* newComponent = &pool_[numSounds_++];
        newComponent->init();
        
        return newComponent;
    }

    void SoundInstancePool::destroyObject(const int index) {
        // Swap with last active object and deactivate
        SoundInstance temp = pool_[--numSounds_];
        pool_[numSounds_] = pool_[index];
        pool_[index] = temp;
    }
    
    // SoundInstance //
    void SoundInstance::init() {

    }
}
