#pragma once

#include <cstdint>

namespace game {
    class GameObject {
        public:
            // Types
            using id_t = uint64_t;

            // Constructors
            GameObject(const GameObject &) = delete;
            GameObject &operator=(const GameObject &) = delete;
            GameObject(GameObject&&) = default;
            GameObject &operator=(GameObject&&) = default;

            // Functions
            static GameObject createGameObject() {
                static id_t currentId = 0;
                return GameObject{currentId++};
            }

            id_t getId() { return id; }
        
        private:
            // Constructors
            GameObject(id_t objId) : id{objId} {}

            // Variables
            id_t id;

    };
}
