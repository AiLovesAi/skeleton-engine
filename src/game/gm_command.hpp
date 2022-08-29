#pragma once

#include "objects/gm_game_object.hpp"

namespace game {
    class Command {
        public:
            virtual ~Command();
            virtual void execute() = 0;
    };
}
