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

            void setParent(const Entity parent) { _parent = parent; }
            void addChild(const Entity child) { _children.push_back(child); }
            void removeChild(const Entity child) { _children.erase(std::find(_children.begin(), _children.end(), child)); }
            void removeFirstChild() { _children.erase(_children.begin()); }
            void removeLastChild() { _children.erase(_children.end()); }

            void setSpeed(const WorldTransform& speedTransform) {
                _speedTransform = speedTransform;
                _speedTransform.dirty = true;
            }
            void setAcceleration(const WorldTransform& accelerationTransform) {
                _accelerationTransform = accelerationTransform;
                _accelerationTransform.dirty = true;
            }

            Entity entity() const { return _entity; }
            Entity parent() const { return _parent; }
            std::vector<Entity> children() const { return _children; }

            WorldTransform speedTransform() const { return _speedTransform; }
            WorldTransform accelerationTransform() const { return _accelerationTransform; }

            // Variables
            static constexpr WorldTransform origin{};
        
        private:
            // Variables
            Entity _entity;

            Entity _parent;
            std::vector<Entity> _children;
            
            WorldTransform _speedTransform{};
            WorldTransform _accelerationTransform{};
    };

    class PhysicsPool {
        public:
            // Constructors
            PhysicsPool(EntityPool& entityPool) : _entityPool{entityPool} {}

            // Functions
            void create(const Entity entity);
            void destroy(const size_t index);
            PhysicsComponent& get(const Entity entity) { return _pool[_indexMap[entity]]; }
            size_t size() const { return _size; };

            void update();

        private:
            // Variables
            EntityPool& _entityPool;
            std::unordered_map<Entity, size_t> _indexMap;
            std::vector<PhysicsComponent> _pool{64};
            size_t _size = 0;
    };
}
