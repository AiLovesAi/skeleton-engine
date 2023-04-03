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
            TransformComponent(const Entity entity) : _entity{entity} {}

            // Functions
            Entity entity() const { return _entity; }
            Entity parent() const { return _parent; }
            std::vector<Entity> children() const { return _children; }

            void setParent(const Entity parent) { _parent = parent; }
            void addChild(const Entity child) { _children.push_back(child); }
            void removeChild(const Entity child) { _children.erase(std::find(_children.begin(), _children.end(), child)); }
            void removeFirstChild() { _children.erase(_children.begin()); }
            void removeLastChild() { _children.erase(_children.end()); }

            void setTransform(const WorldTransform& transform) { _transform = transform; _transform.dirty = true; }
            void setPosition(const glm::dvec3& position) { _transform.position = position; _transform.dirty = true; }
            void setScale(const glm::dvec3& scale) { _transform.scale = scale; _transform.dirty = true; }
            void setRotation(const glm::dquat& rotation) { _transform.rotation = rotation; _transform.dirty = true; }

            WorldTransform transform() const { return _transform; }
            glm::dvec3 position() const { return _transform.position; }
            glm::dvec3 scale() const { return _transform.scale; }
            glm::dquat rotation() const { return _transform.rotation; }
            bool dirty() const { return _transform.dirty; }

            // Variables
            static constexpr WorldTransform origin{};
        
        private:
            // Variables
            Entity _entity;

            Entity _parent;
            std::vector<Entity> _children;
            
            WorldTransform _transform{};
            WorldTransform _speedTransform{};
            WorldTransform _accelerationTransform{};
    };

    class TransformPool {
        public:
            // Constructors
            TransformPool(EntityPool& entityPool, size_t initialCapacity) : _entityPool{entityPool}, _initialCapacity{initialCapacity} {}

            // Functions
            void create(const Entity entity);
            void destroy(const size_t index);
            TransformComponent& get(const Entity entity) { return _pool[_indexMap[entity]]; }
            size_t size() const { return _size; };

        private:
            // Variables
            EntityPool& _entityPool;
            size_t _initialCapacity;
            std::unordered_map<Entity, size_t> _indexMap;
            std::vector<TransformComponent> _pool{_initialCapacity};
            size_t _size = 0;
    };
}
