#pragma once

#include "../components/gm_ai_component.hpp"
#include "../components/gm_physics_component.hpp"

#include <string>

namespace game {
    class World {
        public:
            // Constructors
            World(EntityPool& entityPool);
            ~World();

            World(const World &) = delete;
            World &operator=(const World &) = delete;
            World(World&&) = delete;
            World &operator=(World&&) = delete;

            // Functions
            void load(const std::string& world);
            void update();
            void save();

            PhysicsPool& physicsPool() { return physicsPool_; }
            AIPool& aiPool() { return aiPool_; }

        private:
            EntityPool& entityPool_;
            AIPool aiPool_{entityPool_};
            PhysicsPool physicsPool_{entityPool_};
    };
}