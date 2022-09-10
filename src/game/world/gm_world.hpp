#pragma once

#include "../components/gm_ai_component.hpp"
#include "../components/gm_physics_component.hpp"

#include <string>

namespace game {
    class World {
        public:
            World(const std::string& world);
            ~World();

            World(const World &) = delete;
            World &operator=(const World &) = delete;
            World(World&&) = delete;
            World &operator=(World&&) = delete;

            void update();

        private:
            AIComponentPool aiPool_;
            PhysicsComponentPool physicsPool_;
    };
}