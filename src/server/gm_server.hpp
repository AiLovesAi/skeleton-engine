#pragma once

#include <map>

namespace game {
    class Server {
        public:
            // Constructors
            Server(const Server &) = delete;
            Server &operator=(const Server &) = delete;
            Server(Server&&) = default;
            Server &operator=(Server&&) = default;

            // Functions
            static Server& instance();
            void start();
        
        private:
            // Constructors
            Server();
            ~Server();
    };
}