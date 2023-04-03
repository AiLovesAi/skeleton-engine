#pragma once

#include <server/entities/gm_entity.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace game {
    class AIComponent {
        public:
            // Constructors
            AIComponent(const Entity entity);

            // Functions
            bool update();

            Entity entity() const { return _entity; }
        
        private:
            // Data
            Entity _entity;
    };

    class AIPool {
        public:
            // Constructors
            AIPool(EntityPool& entityPool, const size_t initialCapacity)
                : _entityPool{entityPool}, _initialCapacity{initialCapacity} {}

            // Functions
            void create(const Entity entity);
            void destroy(const size_t index);
            AIComponent& get(const Entity entity) { return _pool[_indexMap[entity]]; }
            size_t size() const { return _size; };

            void update();

        private:
            // Variables
            EntityPool& _entityPool;
            size_t _initialCapacity;
            std::unordered_map<Entity, size_t> _indexMap;
            std::vector<AIComponent> _pool{_initialCapacity};
            size_t _size = 0;
    };
}
