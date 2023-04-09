#pragma once

#include "../headers/string.hpp"

#include <atomic>
#include <string>
#include <thread>
#include <unordered_map>

namespace game {
    class Threads {
        public:
            // Functions
            static void registerThread(const std::thread::id& id, const UTF8Str& name) { _threads[id] = name; }
            static void removeThread(const std::thread::id& id) { _threads.erase(id); }

            static UTF8Str threadName(const std::thread::id& id) {
                return _threads.contains(id) ? _threads.find(id)->second : UTF8Str{"Async", sizeof("Async") - 1};
            }

        private:
            // Variables
            static std::unordered_map<std::thread::id, UTF8Str> _threads;
    };
}
