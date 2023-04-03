#pragma once

#include "../window/gm_window.hpp"

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
                float fov,
                float clipNear,
                float clipFar
            );
        
            // Functions
            void update(const double lag);
            void updateView();
            void updatePerspective();

            const void setFOV(const float fov) { _fov = fov; updatePerspective(); }

            const float fov() const { return _fov; }
            const glm::mat4 projection() const { return _projectionMatrix; }
            const glm::mat4 view() const { return _viewMatrix; }
            
        private:
            // Variables
            static constexpr glm::vec3 _upAxis      {0.f, 1.f, 0.f};
            static constexpr glm::vec3 _forwardAxis {0.f, 0.f, 1.f};

            Entity _entity;
            World& _world;

            Window& _window;

            float _fov;
            float _clipNear;
            float _clipFar;

            glm::mat4 _projectionMatrix{1.0f};
            glm::mat4 _viewMatrix{1.0f};
    };
}
