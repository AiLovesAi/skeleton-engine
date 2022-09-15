#pragma once

#include <string>

namespace game {
    class RenderComponent {
        public:
            // Functions
            void init();
            bool render();
        
        private:
            // Data
    };

    class RenderComponentPool {
        public:
            // Constructors
            RenderComponentPool() {}

            RenderComponentPool(const RenderComponentPool &) = delete;
            RenderComponentPool &operator=(const RenderComponentPool &) = delete;
            RenderComponentPool(RenderComponentPool&&) = delete;
            RenderComponentPool &operator=(RenderComponentPool&&) = delete;

            // Functions
            RenderComponent* createObject();
            void destroyObject(const int index);
            void renderComponents();
        
        private:
            // Variables
            static constexpr int POOL_SIZE = 1024;
            RenderComponent pool_[POOL_SIZE];
            int numComponents_ = 0;
    };
}
