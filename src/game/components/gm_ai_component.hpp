#pragma once

#include <string>

namespace game {
    class AIComponent {
        public:
            // Functions
            void init();
            bool update();
        
        private:
            // Data
    };

    class AIComponentPool {
        public:
            // Constructors
            AIComponentPool() {}

            AIComponentPool(const AIComponentPool &) = delete;
            AIComponentPool &operator=(const AIComponentPool &) = delete;
            AIComponentPool(AIComponentPool&&) = delete;
            AIComponentPool &operator=(AIComponentPool&&) = delete;

            // Functions
            AIComponent* createComponent();
            void destroyComponent(const int index);
            void updateComponents();
        
        private:
            // Variables
            static constexpr int POOL_SIZE = 1024;
            AIComponent pool_[POOL_SIZE];
            int numComponents_ = 0;
    };
}
