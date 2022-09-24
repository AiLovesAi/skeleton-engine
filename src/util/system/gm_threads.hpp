#pragma once

#include <atomic>
#include <string>
#include <thread>
#include <unordered_map>

namespace game {
    class Threads {
        public:
            // Functions
            static void registerThread(const std::thread::id& id, const std::string& name) { threads_[id] = name; }
            static void removeThread(const std::thread::id& id) { threads_.erase(id); }

            static std::string threadName(const std::thread::id& id) {
                return threads_.contains(id) ? threads_.find(id)->second : "Async";
            }

        private:
            // Variables
            static std::unordered_map<std::thread::id, std::string> threads_;
    };
}
