#pragma once

#include "gm_bkv.hpp"

namespace game {
    class BKV_State {
        public:
            // Functions
            virtual void parse(BKVbuf_t& buf, const char c) = 0;
            
        protected:
            // Variables
            BKV_State* parent_ = nullptr; 
    };
}
