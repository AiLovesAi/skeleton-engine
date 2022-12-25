#pragma once

#include "gm_bkv_state.hpp"
#include "../gm_bkv.hpp"
#include "../gm_bkv_buffer.hpp"

namespace game {
    class BKV_State_Array : public BKV_State {
        public:
            // Functions
            void reset() {
                size_ =  0;
                arrayStart_ = 0;
                lastTag_ = 0;
            }
            virtual void parse(BKV_Buffer& buf, const char c);

        private:
            // Variables
            size_t size_ = 0;
            size_t arrayStart_ = 0;
            uint8_t lastTag_ = 0;
    };
}
