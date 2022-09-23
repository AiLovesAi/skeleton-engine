#pragma once

#include "../entities/gm_entity.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>

namespace game {
    // Types
    struct WorldTransform {
        glm::dvec3 position{};
        glm::dvec3 scale{1., 1., 1.};
        glm::dquat rotation{0., 0., 0., 1.};
        bool dirty; // True if the transform has changed this frame and must be recalculated for rendering
    };

    class TransformComponent {
        public:
            // Constructors
            TransformComponent(const Entity entity) : entity_{entity} {}

            // Functions
            Entity entity() const { return entity_; }

            // Variables
            static constexpr WorldTransform origin{};
        
        private:
            // Variables
            Entity entity_;

            TransformComponent* parent_;
            std::vector<TransformComponent*> children_;
            
            WorldTransform transform_{};
            WorldTransform speedTransform_{};
            WorldTransform accelerationTransform_{};
    };

    class TransformPool {
        public:
            // Constructors
            TransformPool(EntityPool& entityPool, size_t initialCapacity) : entityPool_{entityPool}, initialCapacity_{initialCapacity} {}

            // Functions
            void create(TransformComponent& component);
            void destroy(const size_t index);
            TransformComponent& get(const Entity entity) { return pool_[indexMap_[entity]]; }
            size_t size() const { return size_; };

        private:
            // Variables
            EntityPool& entityPool_;
            size_t initialCapacity_;
            std::unordered_map<Entity, size_t> indexMap_;
            std::vector<TransformComponent> pool_{initialCapacity_};
            size_t size_ = 0;
    };
}
