#pragma once

#include "../window/gm_window.hpp"

#include <common/headers/float.hpp>
#include <server/entities/gm_entity.hpp>
#include <server/world/gm_world.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace game {
    class Camera {
        public:
            // Constructors
            Camera(
                const Entity entity,
                World& world,
                Window& window,
                float32_t fov,
                float32_t clipNear,
                float32_t clipFar
            );
        
            // Functions
            void update(const float128_t lag);
            void updateView();
            void updatePerspective();

            const void setFOV(const float32_t fov) { _fov = fov; updatePerspective(); }

            const float32_t fov() const { return _fov; }
            const glm::mat4 projection() const { return _projectionMatrix; }
            const glm::mat4 view() const { return _viewMatrix; }
            
        private:
            // Variables
            static constexpr glm::vec3 _upAxis      {0.f, 1.f, 0.f};
            static constexpr glm::vec3 _forwardAxis {0.f, 0.f, 1.f};

            Entity _entity;
            World& _world;

            Window& _window;

            float32_t _fov;
            float32_t _clipNear;
            float32_t _clipFar;

            glm::mat4 _projectionMatrix{1.0f};
            glm::mat4 _viewMatrix{1.0f};
    };
}
