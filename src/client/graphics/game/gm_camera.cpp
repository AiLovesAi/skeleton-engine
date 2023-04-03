#include "gm_camera.hpp"

#include <common/data/file/gm_logger.hpp>

namespace game {
    Camera::Camera(
        const Entity entity,
        World& world,
        Window& window,
        float fov,
        float clipNear,
        float clipFar
    ) : _entity{entity}, _world{world}, _window{window}, _fov{fov}, _clipNear{clipNear}, _clipFar{clipFar} {
        _world.serverComponents().transform().create(entity);
        updatePerspective();
    }

    void Camera::update(const double lag) {
        if (_window.wasResized()) updatePerspective();
        if (_world.serverComponents().transform().get(_entity).dirty()) updateView();
    }

    void Camera::updatePerspective() {
        _projectionMatrix = glm::perspective(_fov, _window.aspectRatio(), _clipNear, _clipFar);
    }

    void Camera::updateView() {
        // TODO
        //_viewMatrix = glm::mat4_cast(rotation) * _viewMatrix;
    }
}
