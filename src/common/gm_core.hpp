#pragma once

#include "data/string/gm_utf8.hpp"

#include <atomic>
#include <memory>

namespace game {
    class Core{
        public:
            // Functions
            static void init(const char*__restrict__ logFile, const char*__restrict__ crashFile);

            // Variables
            static UTF8Str TITLE;
            static UTF8Str VERSION;

            static constexpr int MS_PER_TICK = 1000 / 20; // 20 TPS
            
            static std::atomic<bool> running;
    };
}
