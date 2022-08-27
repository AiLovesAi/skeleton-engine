#pragma once

#include "gm_game_object.hpp"
#include "../../graphics/gm_model.hpp"

namespace game {
    class Prop : public GameObject {
        public:
            // Variables
            Model& model;
    };
}
