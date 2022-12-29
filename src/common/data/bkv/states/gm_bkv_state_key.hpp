#pragma once

#include "gm_bkv_state.hpp"

namespace game {
    class BKV_Buffer;
    
    class BKV_State_Key : public BKV_State {
        public:
            // Constructors
            BKV_State_Key() {}
            ~BKV_State_Key() {}
            
            // Functions
            void reset() {
                keyLen_ = 0;
                strChar_ = DEFAULT_CHAR;
                breakChar_ = false;
            }
            virtual void parse(BKV_Buffer& buf, const char c);
            
        private:
            // Functions
            void completeKey(BKV_Buffer& buf, const char c);
            void continueKey(BKV_Buffer& buf, const char c);

            // Variables
            static constexpr char DEFAULT_CHAR = -1;
            
            uint8_t key_[UINT8_MAX];
            int16_t keyLen_ = 0;
            char strChar_ = DEFAULT_CHAR;
            bool breakChar_ = false;
    };
}
