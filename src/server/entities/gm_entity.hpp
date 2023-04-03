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
                while (alive(++_next));
                _entities.insert(_next);
                return _next;
            }

            bool alive(Entity entity) {
                return _entities.contains(entity);
            }

            void destroy(Entity entity) {
                _entities.erase(entity);
            }

        private:
            // Variables
            std::unordered_set<Entity> _entities;
            Entity _next;
    };
}
