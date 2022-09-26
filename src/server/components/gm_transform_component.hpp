#pragma once

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
            Entity parent() const { return parent_; }
            std::vector<Entity> children() const { return children_; }

            void setParent(const Entity parent) { parent_ = parent; }
            void addChild(const Entity child) { children_.push_back(child); }
            void removeChild(const Entity child) { children_.erase(std::find(children_.begin(), children_.end(), child)); }
            void removeFirstChild() { children_.erase(children_.begin()); }
            void removeLastChild() { children_.erase(children_.end()); }

            void setTransform(const WorldTransform& transform) { transform_ = transform; transform_.dirty = true; }
            void setPosition(const glm::dvec3& position) { transform_.position = position; transform_.dirty = true; }
            void setScale(const glm::dvec3& scale) { transform_.scale = scale; transform_.dirty = true; }
            void setRotation(const glm::dquat& rotation) { transform_.rotation = rotation; transform_.dirty = true; }

            WorldTransform transform() const { return transform_; }
            glm::dvec3 position() const { return transform_.position; }
            glm::dvec3 scale() const { return transform_.scale; }
            glm::dquat rotation() const { return transform_.rotation; }
            bool dirty() const { return transform_.dirty; }

            // Variables
            static constexpr WorldTransform origin{};
        
        private:
            // Variables
            Entity entity_;

            Entity parent_;
            std::vector<Entity> children_;
            
            WorldTransform transform_{};
            WorldTransform speedTransform_{};
            WorldTransform accelerationTransform_{};
    };

    class TransformPool {
        public:
            // Constructors
            TransformPool(EntityPool& entityPool, size_t initialCapacity) : entityPool_{entityPool}, initialCapacity_{initialCapacity} {}

            // Functions
            void create(const Entity entity);
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
