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
            Sound* sound() const { return _sound; }
            Entity entity() const { return _entity; }
            float pitch() const { return _pitch; }
            float volume() const { return _volume; }
            float position() const { return _position; }

        private:
            // Variables
            Sound* _sound = nullptr;
            Entity _entity;

            float _pitch = 1.f;
            float _volume = 1.f;
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
