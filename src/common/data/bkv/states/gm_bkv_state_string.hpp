#pragma once

#include "gm_bkv_state.hpp"

#include <memory>

namespace game {
    class BKV_Buffer;
    
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
                strChar_ = 0;
                breakChar_ = false;
            }
            static char getBreakChar(const char c);
            virtual void parse(BKV_Buffer& buf, const char c);
            
        private:
            // Functions
            void continueStr(BKV_Buffer& buf, const char c);
            void checkForBool(BKV_Buffer& buf);
            void completeStr(BKV_Buffer& buf, const char c);
            
            // Variables
            uint8_t* str_ = nullptr;
            int64_t strCapacity_ = 0;
            int64_t strLen_ = 0;
            char strChar_ = 0;
            bool breakChar_ = false;
    };
}
