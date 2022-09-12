#include "gm_camera.hpp"

#include "../../util/gm_logger.hpp"

namespace game {
    Camera::Camera(
        PhysicsComponent*const physics,
        Window*const window,
        float fov,
        float clipNear,
        float clipFar
    ) : physics_{physics}, window_{window}, fov_{fov}, clipNear_{clipNear}, clipFar_{clipFar} {
        if (physics == nullptr) Logger::crash("Physics component passed to camera was null.");
        if (window == nullptr) Logger::crash("Window passed to camera was null.");
        updatePerspective();
    }

    void Camera::update() {
        if (window_->wasResized()) updatePerspective();
        if (physics_->dirty()) updateView();
    }

    void Camera::updatePerspective() {
        projectionMatrix_ = glm::perspective(fov_, window_->aspectRatio(), clipNear_, clipFar_);
    }

    void Camera::updateView() {
        // TODO
        //viewMatrix_ = glm::mat4_cast(rotation) * viewMatrix_;
    }
}
