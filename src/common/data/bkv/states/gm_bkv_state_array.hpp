#pragma once

#include "gm_bkv_state.hpp"
#include "../gm_bkv.hpp"
#include "../gm_bkv_buffer.hpp"

namespace game {
    class BKV_State_Array : public BKV_State {
        public:
            // Functions
            void reset() {
                arrayStart_ = 0;
            }
            virtual void parse(BKV_Buffer& buf, const char c);

        private:
            // Variables
            size_t arrayStart_ = 0;
    };
}
