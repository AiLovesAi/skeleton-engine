#pragma once

#include "gm_bkv_state.hpp"
#include "../gm_bkv_buffer.hpp"

namespace game {
    class BKV_State_Complete : public BKV_State {
        public:
            // Functions
            virtual void parse(BKV_Buffer& buf, const char c);
    };
}