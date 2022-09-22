#pragma once

#include "gm_physics_component.hpp"
#include "../entities/gm_entity.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace game {
    class RenderComponent {
        public:
            // Constructors
            RenderComponent(const Entity entity);

            // Functions
            void render(const WorldTransform& parentTransform, bool dirty);

            Entity entity() const { return entity_; }
        
        private:
            // Variables
            Entity entity_;
    };
    
    class RenderPool {
        public:
            // Constructors
            RenderPool(EntityPool& entityPool) : entityPool_{entityPool} {}

            // Functions
            void create(RenderComponent& component);
            void destroy(const size_t index);
            RenderComponent& get(const Entity entity) { return pool_[indexMap_[entity]]; }
            size_t size() const { return size_; };

            void render();

        private:
            // Variables
            EntityPool& entityPool_;
            std::unordered_map<Entity, size_t> indexMap_;
            std::vector<RenderComponent> pool_{64};
            size_t size_ = 0;
    };
}
