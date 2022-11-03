#pragma once

#include "gm_bkv_state.hpp"
#include "../gm_bkv.hpp"
#include "../gm_bkv_buffer.hpp"

#include <memory>

namespace game {
    class BKV_State_String : public BKV_State {
        public:
            // Constructors
            BKV_State_String() {
                strCapacity_ = BUFSIZ;
                str_ = static_cast<uint8_t*>(std::malloc(strCapacity_));
            }
            ~BKV_State_String() { std::free(str_); }

            // Functions
            void reset() {
                strLen_ = 0;
                strChar_ = DEFAULT_CHAR;
                lastChar_ = DEFAULT_CHAR;
            }
            virtual void parse(BKV_Buffer& buf, const char c);
            
        private:
            // Functions
            void continueStr(BKV_Buffer& buf, const char c);
            
            // Variables
            static constexpr char DEFAULT_CHAR = -1;
            
            uint8_t* str_ = nullptr;
            size_t strCapacity_ = 0;
            size_t strLen_ = 0;
            char strChar_ = DEFAULT_CHAR;
            char lastChar_ = DEFAULT_CHAR;
    };
}
