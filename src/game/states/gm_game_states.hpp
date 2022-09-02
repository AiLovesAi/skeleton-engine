#pragma once

#include "../../graphics/gm_renderer.hpp"

namespace game {
    class GameState {
        public:
            GameState() {}
            virtual ~GameState() {}

            virtual void update() {}
            virtual void render() {}
    };

    class MenuState : public GameState {
        public:
            MenuState(){}

            virtual void update();
            virtual void render();

        private:
            // Menu GUI
    };

    class HostState : public GameState {
        public:
            HostState() {}

            virtual void update();
            virtual void render();
    };

    class ClientState : public GameState {
        public:
            ClientState() {}

            virtual void update();
            virtual void render();
    };
}
