#pragma once

#include "../commands/gm_command.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <vector>

namespace game {
    class PhysicsComponent {
        public:
            // Types
            struct WorldTransformComponent {
                glm::dvec3 position{};
                glm::dvec3 scale{1., 1., 1.};
                glm::dquat rotation{0., 0., 0., 1.};
                bool dirty; // True if the transform has changed this frame and must be recalculated for rendering
            };

            // Functions
            void init();
            bool update();

            void setParent(PhysicsComponent*const parent) { parent_ = parent; }
            void addChild(PhysicsComponent*const child) { children_.push_back(child); }
            void removeChild(PhysicsComponent*const child) { children_.erase(std::find(children_.begin(), children_.end(), child)); }
            void removeFirstChild(PhysicsComponent*const child) { children_.erase(children_.begin()); }
            void removeLastChild(PhysicsComponent*const child) { children_.erase(children_.end()); }

            void setTransform(const WorldTransformComponent& transform) { transform_ = transform; transform_.dirty = true; }
            void setPosition(const glm::dvec3& position) { transform_.position = position; transform_.dirty = true; }
            void setScale(const glm::dvec3& scale) { transform_.scale = scale; transform_.dirty = true; }
            void setRotation(const glm::dquat& rotation) { transform_.rotation = rotation; transform_.dirty = true; }

            void setSpeed(const WorldTransformComponent& speedTransform) {
                speedTransform_ = speedTransform;
                speedTransform_.dirty = true;
            }
            void setAcceleration(const WorldTransformComponent& accelerationTransform) {
                accelerationTransform_ = accelerationTransform;
                accelerationTransform_.dirty = true;
            }

            PhysicsComponent* parent() const { return parent_; }
            std::vector<PhysicsComponent*> children() const { return children_; }

            WorldTransformComponent transform() const { return transform_; }
            glm::dvec3 position() const { return transform_.position; }
            glm::dvec3 scale() const { return transform_.scale; }
            glm::dquat rotation() const { return transform_.rotation; }
            bool dirty() const { return transform_.dirty; }

            WorldTransformComponent speedTransform() const { return speedTransform_; }
            WorldTransformComponent accelerationTransform() const { return accelerationTransform_; }
        
        private:
            // Variables
            PhysicsComponent* parent_;
            std::vector<PhysicsComponent*> children_;
            
            WorldTransformComponent transform_{};
            WorldTransformComponent speedTransform_{};
            WorldTransformComponent accelerationTransform_{};
    };

    class PhysicsComponentPool {
        public:
            // Constructors
            PhysicsComponentPool() {}

            PhysicsComponentPool(const PhysicsComponentPool &) = delete;
            PhysicsComponentPool &operator=(const PhysicsComponentPool &) = delete;
            PhysicsComponentPool(PhysicsComponentPool&&) = delete;
            PhysicsComponentPool &operator=(PhysicsComponentPool&&) = delete;

            // Functions
            PhysicsComponent* createComponent();
            void destroyComponent(const int index);
            void updateComponents();
        
        private:
            // Variables
            static constexpr int POOL_SIZE = 1024;
            PhysicsComponent pool_[POOL_SIZE];
            int numComponents_ = 0;
    };

    class CmdPhysicsComponentMove : Command {
        public:
            virtual void execute(PhysicsComponent& actor, const glm::dvec3& amount) {
                actor.setPosition(actor.transform().position + amount);
            }
    };
}
