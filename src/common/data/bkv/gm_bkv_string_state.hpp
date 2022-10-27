#pragma once

#include "gm_bkv.hpp"

namespace game {
    class BKV_String_State : public BKV_State {
        public:
            // Functions
            virtual void parse(BKVbuf_t& buf, const char c);
            
        private:
            // Variables
            static constexpr char DEFAULT_CHAR = -1;
            char strChar_ = DEFAULT_CHAR;
    };
}
