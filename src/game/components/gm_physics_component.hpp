#pragma once

#include <string>

namespace game {
    class PhysicsComponent {
        public:
            // Functions
            void init();
            bool update();
        
        private:
            // Data
    };

    class PhysicsComponentPool {
        public:
            // Constructors
            PhysicsComponentPool() {}

            PhysicsComponentPool(const PhysicsComponentPool &) = delete;
            PhysicsComponentPool &operator=(const PhysicsComponentPool &) = delete;
            PhysicsComponentPool(PhysicsComponentPool&&) = delete;
            PhysicsComponentPool &operator=(PhysicsComponentPool&&) = delete;

            // Functions
            PhysicsComponent* createComponent();
            void destroyComponent(const int index);
            void updateComponents();
        
        private:
            // Variables
            static constexpr int POOL_SIZE = 1024;
            PhysicsComponent pool_[POOL_SIZE];
            int numComponents_ = 0;
    };
}
