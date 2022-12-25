#pragma once

#include "gm_bkv_state.hpp"
#include "../gm_bkv.hpp"
#include "../gm_bkv_buffer.hpp"

namespace game {
    class BKV_State_Name : public BKV_State {
        public:
            // Functions
            void reset() {
                nameLen_ = 0;
                lastChar_ = DEFAULT_CHAR;
            }
            virtual void parse(BKV_Buffer& buf, const char c);
            
        private:
            // Variables
            static constexpr char DEFAULT_CHAR = -1;
            uint8_t name_[UINT8_MAX];
            size_t nameLen_ = 0;
            char lastChar_ = DEFAULT_CHAR;
    };
}
