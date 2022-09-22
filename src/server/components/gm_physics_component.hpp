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

    class PhysicsComponent {
        public:
            // Constructors
            PhysicsComponent(const Entity entity);

            // Functions
            bool update();

            void setParent(PhysicsComponent*const parent) { parent_ = parent; }
            void addChild(PhysicsComponent*const child) { children_.push_back(child); }
            void removeChild(PhysicsComponent*const child) { children_.erase(std::find(children_.begin(), children_.end(), child)); }
            void removeFirstChild(PhysicsComponent*const child) { children_.erase(children_.begin()); }
            void removeLastChild(PhysicsComponent*const child) { children_.erase(children_.end()); }

            void setTransform(const WorldTransform& transform) { transform_ = transform; transform_.dirty = true; }
            void setPosition(const glm::dvec3& position) { transform_.position = position; transform_.dirty = true; }
            void setScale(const glm::dvec3& scale) { transform_.scale = scale; transform_.dirty = true; }
            void setRotation(const glm::dquat& rotation) { transform_.rotation = rotation; transform_.dirty = true; }

            void setSpeed(const WorldTransform& speedTransform) {
                speedTransform_ = speedTransform;
                speedTransform_.dirty = true;
            }
            void setAcceleration(const WorldTransform& accelerationTransform) {
                accelerationTransform_ = accelerationTransform;
                accelerationTransform_.dirty = true;
            }

            Entity entity() const { return entity_; }

            PhysicsComponent* parent() const { return parent_; }
            std::vector<PhysicsComponent*> children() const { return children_; }

            WorldTransform transform() const { return transform_; }
            glm::dvec3 position() const { return transform_.position; }
            glm::dvec3 scale() const { return transform_.scale; }
            glm::dquat rotation() const { return transform_.rotation; }
            bool dirty() const { return transform_.dirty; }

            WorldTransform speedTransform() const { return speedTransform_; }
            WorldTransform accelerationTransform() const { return accelerationTransform_; }

            // Variables
            static constexpr WorldTransform origin{};
        
        private:
            // Variables
            Entity entity_;

            PhysicsComponent* parent_;
            std::vector<PhysicsComponent*> children_;
            
            WorldTransform transform_{};
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
