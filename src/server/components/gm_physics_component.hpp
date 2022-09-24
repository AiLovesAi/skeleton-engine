#pragma once

#include "gm_transform_component.hpp"

#include <server/entities/gm_entity.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>

namespace game {
    class PhysicsComponent {
        public:
            // Constructors
            PhysicsComponent(const Entity entity);

            // Functions
            bool update();

            void setParent(const Entity parent) { parent_ = parent; }
            void addChild(const Entity child) { children_.push_back(child); }
            void removeChild(const Entity child) { children_.erase(std::find(children_.begin(), children_.end(), child)); }
            void removeFirstChild() { children_.erase(children_.begin()); }
            void removeLastChild() { children_.erase(children_.end()); }

            void setSpeed(const WorldTransform& speedTransform) {
                speedTransform_ = speedTransform;
                speedTransform_.dirty = true;
            }
            void setAcceleration(const WorldTransform& accelerationTransform) {
                accelerationTransform_ = accelerationTransform;
                accelerationTransform_.dirty = true;
            }

            Entity entity() const { return entity_; }
            Entity parent() const { return parent_; }
            std::vector<Entity> children() const { return children_; }

            WorldTransform speedTransform() const { return speedTransform_; }
            WorldTransform accelerationTransform() const { return accelerationTransform_; }

            // Variables
            static constexpr WorldTransform origin{};
        
        private:
            // Variables
            Entity entity_;

            Entity parent_;
            std::vector<Entity> children_;
            
            WorldTransform speedTransform_{};
            WorldTransform accelerationTransform_{};
    };

    class PhysicsPool {
        public:
            // Constructors
            PhysicsPool(EntityPool& entityPool) : entityPool_{entityPool} {}

            // Functions
            void create(PhysicsComponent& component);
            void destroy(const size_t index);
            PhysicsComponent& get(const Entity entity) { return pool_[indexMap_[entity]]; }
            size_t size() const { return size_; };

            void update();

        private:
            // Variables
            EntityPool& entityPool_;
            std::unordered_map<Entity, size_t> indexMap_;
            std::vector<PhysicsComponent> pool_{64};
            size_t size_ = 0;
    };
}
