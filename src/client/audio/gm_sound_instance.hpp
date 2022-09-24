#pragma once

#include "gm_sound.hpp"

#include <server/entities/gm_entity.hpp>

#include <unordered_map>
#include <vector>

namespace game {
    class SoundInstance {
        public:
            // Constructors
            SoundInstance(const Entity entity);

            // Functions
            Sound* sound() const { return sound_; }
            Entity entity() const { return entity_; }
            float pitch() const { return pitch_; }
            float volume() const { return volume_; }
            float position() const { return position_; }

        private:
            // Variables
            Sound* sound_ = nullptr;
            Entity entity_;

            float pitch_ = 1.f;
            float volume_ = 1.f;
            size_t position_ = 0; // Position in sound data
    };
    
    class SoundPool {
        public:
            // Constructors
            SoundPool(EntityPool& entityPool, const size_t initialCapacity)
                : entityPool_{entityPool}, initialCapacity_{initialCapacity} {}

            // Functions
            void create(SoundInstance& instance);
            void destroy(const size_t index);
            SoundInstance& get(const Entity entity) { return pool_[indexMap_[entity]]; }
            size_t size() const { return size_; };

        private:
            // Variables
            EntityPool& entityPool_;
            size_t initialCapacity_;
            std::unordered_map<Entity, size_t> indexMap_;
            std::vector<SoundInstance> pool_{initialCapacity_};
            size_t size_ = 0;
    };
}
