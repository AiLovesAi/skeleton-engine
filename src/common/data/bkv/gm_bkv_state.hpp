#pragma once

#include "gm_bkv.hpp"

namespace game {
    class BKV_State {
        public:
            // Functions
            virtual void parse(BKV::BKVbuf_t& buf, const char c) = 0;
            void reset(BKV::BKVbuf_t& buf) {
                buf.bufSize = 0;
            }
            
        protected:
            // Variables
            BKV_State* parent_ = nullptr; 
    };
}
