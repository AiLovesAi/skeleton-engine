#pragma once

#include <cstdint>
#include <unordered_set>

namespace game {
    typedef size_t Entity;

    class EntityPool {
        public:
            // Functions
            Entity create() {
                // Recycle ID if ID loops
                while (alive(++next_));
                entities_.insert(next_);
                return next_;
            }

            bool alive(Entity entity) {
                return entities_.contains(entity);
            }

            void destroy(Entity entity) {
                entities_.erase(entity);
            }

        private:
            // Variables
            std::unordered_set<Entity> entities_;
            Entity next_;
    };
}
