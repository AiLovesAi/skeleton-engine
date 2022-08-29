#pragma once

#include "../gm_client.hpp"

namespace game {
    class GameState {
        public:
            virtual ~GameState() {}

            virtual void update() {}
            virtual void render() {}
    };

    class MenuState : public GameState {
        public:
            MenuState(Client& client) : client{client} {}

            virtual void update();
            virtual void render();

        private:
            Client& client;
            // Menu GUI
    };

    class HostState : public GameState {
        public:
            HostState(Client& client) : client{client} {}

            virtual void update();
            virtual void render();

        private:
            Client& client;
    };

    class ClientState : public GameState {
        public:
            ClientState(Client& client) : client{client} {}

            virtual void update();
            virtual void render();

        private:
            Client& client;
    };
}
