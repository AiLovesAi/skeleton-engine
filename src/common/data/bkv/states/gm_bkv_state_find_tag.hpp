#pragma once

#include "gm_bkv_state.hpp"
#include "../gm_bkv_buffer.hpp"

namespace game {
    class BKV_State_Find_Tag : public BKV_State {
        public:
            // Constructors
            BKV_State_Find_Tag();
            ~BKV_State_Find_Tag();

            // Functions
            virtual void parse(BKV_Buffer& buf, const char c);
    };
}
