#pragma once

#include <server/components/gm_physics_component.hpp>
#include <server/entities/gm_entity.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace game {
    class RenderComponent {
        public:
            // Constructors
            RenderComponent(const Entity entity);

            // Functions
            void render(const double lag, const WorldTransform& parentTransform, bool dirty);

            Entity entity() const { return _entity; }
        
        private:
            // Variables
            Entity _entity;
    };
    
    class RenderPool {
        public:
            // Constructors
            RenderPool(EntityPool& entityPool, const size_t initialCapacity)
                : _entityPool{entityPool}, _initialCapacity{initialCapacity} {}

            // Functions
            void create(const Entity entity);
            void destroy(const size_t index);
            RenderComponent& get(const Entity entity) { return _pool[_indexMap[entity]]; }
            size_t size() const { return _size; };

            void render(const double lag);

        private:
            // Variables
            EntityPool& _entityPool;
            size_t _initialCapacity;
            std::unordered_map<Entity, size_t> _indexMap;
            std::vector<RenderComponent> _pool{_initialCapacity};
            size_t _size = 0;
    };
}
