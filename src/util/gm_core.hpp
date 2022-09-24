#pragma once

#include <atomic>

namespace game {
    class Core{
        public:
            // Functions
            static void init();

            // Variables
            static constexpr char EMPTYSTR[] = "NULL";
            
            static std::string TITLE;
            static std::string VERSION;

            static constexpr int MS_PER_TICK = 1000 / 20; // 20 TPS
            
            static std::atomic<bool> running;
    };
}
