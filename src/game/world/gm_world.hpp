#pragma once

#include "../components/gm_ai_component.hpp"
#include "../components/gm_physics_component.hpp"

#include <string>

namespace game {
    class World {
        public:
            // Constructors
            World();
            ~World();

            World(const World &) = delete;
            World &operator=(const World &) = delete;
            World(World&&) = delete;
            World &operator=(World&&) = delete;

            // Functions
            void load(const std::string& world);
            void update();
            void save();
            
            PhysicsComponent* createPhysicsComponent();
            AIComponent* createAIComponent();

        private:
            AIComponentPool aiPool_;
            PhysicsComponentPool physicsPool_;
    };
}