#pragma once

#include "gm_bkv_state.hpp"
#include "../gm_bkv.hpp"
#include "../gm_bkv_buffer.hpp"

namespace game {
    class BKV_State_Number : public BKV_State { 
        public:
            // Functions
            void reset() {
                bufLen_ = 0;
                hasDecimal_ = false;
            }
            virtual void parse(BKV_Buffer& buf, const char c);

        private:
            // Variables
            char numBuf_[UINT8_MAX];
            size_t bufLen_ = 0;
            bool hasDecimal_ = false;
    };
}
