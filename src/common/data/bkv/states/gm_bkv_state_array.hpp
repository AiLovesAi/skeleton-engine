#pragma once

#include "gm_bkv_state.hpp"
#include "../gm_bkv_buffer.hpp"

namespace game {
    class BKV_State_Array : public BKV_State {
        public:
            // Functions
            void reset() {
                size_ =  0;
                arrayTagHead_ = 0;
                arrayStart_ = 0;
                arrayTag_ = 0;
            }
            virtual void parse(BKV_Buffer& buf, const char c);

        private:
            // Variables
            int64_t size_ = 0;
            int64_t arrayTagHead_ = 0;
            int64_t arrayStart_ = 0;
            uint8_t arrayTag_ = 0;
    };
}
