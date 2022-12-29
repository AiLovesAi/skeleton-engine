#pragma once

#include "gm_bkv_state.hpp"

namespace game {
    class BKV_Buffer;
    
    class BKV_State_Complete : public BKV_State {
        public:
            // Constructors
            BKV_State_Complete() {}
            ~BKV_State_Complete() {}

            // Functions
            virtual void parse(BKV_Buffer& buf, const char c);
    };
}