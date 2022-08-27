#pragma once

#include "../game/gm_game_object.hpp"

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
                GameObject& cameraObject,
                float fov,
                float aspectRatio,
                float clipNear,
                float clipFar
            ) : cameraObject{cameraObject}, fov{fov}, aspectRatio{aspectRatio}, clipNear{clipNear}, clipFar{clipFar} {}
        
            // Functions
            void updateView(const glm::quat rotation) { viewMatrix = glm::mat4_cast(rotation) * viewMatrix; }
            void updatePerspective() { projectionMatrix = glm::perspective(fov, aspectRatio, clipNear, clipFar); }

            const void setFOV(const float fov) { this->fov = fov; updatePerspective(); }
            const void setAspectRatio(const float aspectRatio) { this->aspectRatio = aspectRatio; updatePerspective(); }
            const glm::mat4 &getProjection() const { return projectionMatrix; }
            const glm::mat4 &getView() const { return viewMatrix; }

            // Variables
            GameObject& cameraObject;
            
        private:
            // Variables
            static constexpr glm::vec3 upAxis      {0.f, 1.f, 0.f};
            static constexpr glm::vec3 forwardAxis {0.f, 0.f, 1.f};

            float fov;
            float aspectRatio;
            float clipNear;
            float clipFar;

            glm::mat4 projectionMatrix{1.0f};
            glm::mat4 viewMatrix{1.0f};
    };
}
