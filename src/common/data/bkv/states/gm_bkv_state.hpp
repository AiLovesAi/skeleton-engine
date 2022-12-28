#pragma once

#include <cctype>
#include <sstream>
#include <stdexcept>

namespace game {
    class BKV_Buffer;
    
    class BKV_State {
        public:
            // Functions
            virtual void parse(BKV_Buffer& buf, const char c) = 0;
    };
}
