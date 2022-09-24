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

            Entity entity() const { return entity_; }
        
        private:
            // Variables
            Entity entity_;
    };
    
    class RenderPool {
        public:
            // Constructors
            RenderPool(EntityPool& entityPool, const size_t initialCapacity)
                : entityPool_{entityPool}, initialCapacity_{initialCapacity} {}

            // Functions
            void create(RenderComponent& component);
            void destroy(const size_t index);
            RenderComponent& get(const Entity entity) { return pool_[indexMap_[entity]]; }
            size_t size() const { return size_; };

            void render(const double lag);

        private:
            // Variables
            EntityPool& entityPool_;
            size_t initialCapacity_;
            std::unordered_map<Entity, size_t> indexMap_;
            std::vector<RenderComponent> pool_{initialCapacity_};
            size_t size_ = 0;
    };
}
