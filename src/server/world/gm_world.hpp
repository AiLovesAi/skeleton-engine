#pragma once

#include "../components/gm_server_components.hpp"

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

            ServerComponents& serverComponents() { return _serverComponents; }

        private:
            // Variables
            EntityPool& _entityPool;
            ServerComponents _serverComponents{_entityPool};
    };
}