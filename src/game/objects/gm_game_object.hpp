#pragma once

#include "../gm_command.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>

namespace game {
    class GameObject {
        public:
            // Types
            using id_t = uint64_t;

            struct WorldTransformComponent {
                glm::dvec3 position{};
                glm::dvec3 scale{1., 1., 1.};
                glm::dquat rotation{0., 0., 0., 1.};
            };

            // Constructors
            GameObject(const GameObject &) = delete;
            GameObject &operator=(const GameObject &) = delete;
            GameObject(GameObject&&) = default;
            GameObject &operator=(GameObject&&) = default;

            // Functions
            static GameObject createGameObject() {
                static id_t currentId = 0;
                return GameObject{currentId++};
            }

            id_t getId() { return id; }
            
            // Object variables
            WorldTransformComponent transform{};
            WorldTransformComponent speedTransform{};
            WorldTransformComponent accelerationTransform{};
            GameObject* parent = nullptr;
        
        private:
            GameObject(id_t objId) : id{objId} {}
            id_t id;
    };
    class CmdGameObjectMove : Command {
        public:
            virtual void execute(GameObject& actor, const glm::dvec3& amount) { actor.transform.position += amount; }
    };
}