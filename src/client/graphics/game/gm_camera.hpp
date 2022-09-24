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

            const void setFOV(const float fov) { fov_ = fov; updatePerspective(); }

            const float fov() const { return fov_; }
            const glm::mat4 projection() const { return projectionMatrix_; }
            const glm::mat4 view() const { return viewMatrix_; }
            
        private:
            // Variables
            static constexpr glm::vec3 upAxis_      {0.f, 1.f, 0.f};
            static constexpr glm::vec3 forwardAxis_ {0.f, 0.f, 1.f};

            Entity entity_;
            World& world_;

            Window& window_;

            float fov_;
            float clipNear_;
            float clipFar_;

            glm::mat4 projectionMatrix_{1.0f};
            glm::mat4 viewMatrix_{1.0f};
    };
}
