#pragma once

#include "gm_bkv_state.hpp"

namespace game {
    class BKV;
    class BKV_Buffer;

    class BKV_State_Find_Tag : public BKV_State {
        public:
            // Functions
            virtual void parse(BKV_Buffer& buf, const char c);
        
        private:
            // Functions
            void parseStr(BKV_Buffer& buf, const char c);
    };
}
