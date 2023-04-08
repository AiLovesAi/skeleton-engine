#pragma once

#include "gm_sound.hpp"

#include <common/headers/float.hpp>
#include <server/entities/gm_entity.hpp>

#include <unordered_map>
#include <vector>

namespace game {
    class SoundInstance {
        public:
            // Constructors
            SoundInstance(const Entity entity);

            // Functions
            Sound* sound() const { return _sound; }
            Entity entity() const { return _entity; }
            float32_t pitch() const { return _pitch; }
            float32_t volume() const { return _volume; }
            float32_t position() const { return _position; }

        private:
            // Variables
            Sound* _sound = nullptr;
            Entity _entity;

            float32_t _pitch = 1.f;
            float32_t _volume = 1.f;
            size_t _position = 0; // Position in sound data
    };
    
    class SoundPool {
        public:
            // Constructors
            SoundPool(EntityPool& entityPool, const size_t initialCapacity)
                : _entityPool{entityPool}, _initialCapacity{initialCapacity} {}

            // Functions
            void create(const Entity entity);
            void destroy(const size_t index);
            SoundInstance& get(const Entity entity) { return _pool[_indexMap[entity]]; }
            size_t size() const { return _size; };

        private:
            // Variables
            EntityPool& _entityPool;
            size_t _initialCapacity;
            std::unordered_map<Entity, size_t> _indexMap;
            std::vector<SoundInstance> _pool{_initialCapacity};
            size_t _size = 0;
    };
}
