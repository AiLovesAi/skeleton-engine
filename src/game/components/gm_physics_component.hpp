#pragma once

namespace game {
    class PhysicsComponent {
        public:
            // Constructors
            PhysicsComponent();
            ~PhysicsComponent();

            PhysicsComponent(const PhysicsComponent&) = delete;
            PhysicsComponent& operator=(const PhysicsComponent&) = delete;
            PhysicsComponent(PhysicsComponent&&) = default;
            PhysicsComponent& operator=(PhysicsComponent&&) = default;
    };
}
