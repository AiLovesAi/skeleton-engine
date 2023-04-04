#pragma once

#include <cctype>
#include <stdexcept>

namespace game {
    class BKV_Parser;
    
    class BKV_Parser_State {
        public:
            // Functions
            virtual void parse(BKV_Parser& parser, const char c) = 0;
    };
}
