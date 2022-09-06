#pragma once

#include "../../graphics/gm_renderer.hpp"

#include <chrono>

namespace game {
    class GameState {
        public:
            GameState() {}
            virtual ~GameState() {}

            virtual void load() {}
            virtual void update() {}
            virtual void render(const double lag) {}
    };

    class MenuState : public GameState {
        public:
            MenuState() {}

            virtual void load();
            virtual void update();
            virtual void render(const double lag);

        private:
            // Menu GUI
    };

    class HostState : public GameState {
        public:
            HostState() {}

            virtual void load();
            virtual void update();
            virtual void render(const double lag);
    };

    class ClientState : public GameState {
        public:
            ClientState() {}

            virtual void load();
            virtual void update();
            virtual void render(const double lag);
    };
}
