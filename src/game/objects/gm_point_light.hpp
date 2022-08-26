#pragma once

#include "gm_game_object.hpp"

namespace game {
    class PointLight : public GameObject {
        public:
            glm::vec4 color; // w for intensity
    };
}
