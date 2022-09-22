#pragma once

#include "../entities/gm_entity.hpp"

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

            Entity entity() const { return entity_; }
        
        private:
            // Data
            Entity entity_;
    };

    class AIPool {
        public:
            // Constructors
            AIPool(EntityPool& entityPool, size_t initialCapacity)
                : entityPool_{entityPool}, initialCapacity_{initialCapacity} {}

            // Functions
            void create(AIComponent& component);
            void destroy(const size_t index);
            AIComponent& get(const Entity entity) { return pool_[indexMap_[entity]]; }
            size_t size() const { return size_; };

            void update();

        private:
            // Variables
            EntityPool& entityPool_;
            size_t initialCapacity_;
            std::unordered_map<Entity, size_t> indexMap_;
            std::vector<AIComponent> pool_{initialCapacity_};
            size_t size_ = 0;
    };
}
