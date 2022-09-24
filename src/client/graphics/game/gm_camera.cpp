#include "gm_camera.hpp"

#include <core/logger/gm_logger.hpp>

namespace game {
    Camera::Camera(
        const Entity entity,
        World& world,
        Window& window,
        float fov,
        float clipNear,
        float clipFar
    ) : entity_{entity}, world_{world}, window_{window}, fov_{fov}, clipNear_{clipNear}, clipFar_{clipFar} {
        updatePerspective();
    }

    void Camera::update() {
        if (window_.wasResized()) updatePerspective();
        if (world_.serverComponents().transform().get(entity_).dirty()) updateView();
    }

    void Camera::updatePerspective() {
        projectionMatrix_ = glm::perspective(fov_, window_.aspectRatio(), clipNear_, clipFar_);
    }

    void Camera::updateView() {
        // TODO
        //viewMatrix_ = glm::mat4_cast(rotation) * viewMatrix_;
    }
}
