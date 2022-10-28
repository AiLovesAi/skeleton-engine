#pragma once

#include "gm_bkv.hpp"
#include "gm_bkv_state.hpp"

#include <memory>

namespace game {
    class BKV_State_String : public BKV_State {
        public:
            // Constructors
            BKV_State_String() {
                strCapacity_ = BUFSIZ;
                str_ = static_cast<char*>(std::malloc(strCapacity_));
            }
            ~BKV_State_String() { std::free(str_); }

            // Functions
            void reset() {
                strLen_ = 0;
                strChar_ = DEFAULT_CHAR;
                lastChar_ = DEFAULT_CHAR;
            }
            virtual void parse(BKV::BKVbuf_t& buf, const char c);
            
        private:
            // Variables
            static constexpr char DEFAULT_CHAR = -1;
            
            char* str_ = nullptr;
            size_t strCapacity_ = 0, strLen_ = 0;
            char strChar_ = DEFAULT_CHAR;
            char lastChar_ = DEFAULT_CHAR;
    };
}
