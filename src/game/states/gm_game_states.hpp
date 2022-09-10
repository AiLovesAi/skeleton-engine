#pragma once

#include "../../graphics/gm_renderer.hpp"
#include "../world/gm_world.hpp"

namespace game {
    class GameState {
        public:
            virtual void load() {}
            virtual void unload() {}
            virtual void update() {}
            virtual void render(const double lag) {}
    };

    class MenuState : public GameState {
        public:
            virtual void load();
            virtual void unload();
            virtual void update();
            virtual void render(const double lag);

        private:
            // Menu GUI
    };

    class HostState : public GameState {
        public:
            virtual void load(std::string& worldName);
            virtual void unload();
            virtual void update();
            virtual void render(const double lag);

        private:
            World* world_;
    };

    class ClientState : public GameState {
        public:
            virtual void load();
            virtual void unload();
            virtual void update();
            virtual void render(const double lag);
    };
}
