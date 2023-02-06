#pragma once

#include "gm_bkv_parser_state.hpp"

namespace game {
    class BKV_Parser;
    
    class BKV_Parser_State_Key : public BKV_Parser_State {
        public:
            // Constructors
            BKV_Parser_State_Key() {}
            ~BKV_Parser_State_Key() {}
            
            // Functions
            void reset() {
                keyLen_ = 0;
                strChar_ = 0;
                escapeChar_ = false;
            }
            virtual void parse(BKV_Parser& parser, const char c);
            
        private:
            // Functions
            void completeKey(BKV_Parser& parser, const char c);
            void continueKey(BKV_Parser& parser, const char c);

            // Variables
            uint8_t key_[UINT8_MAX];
            int16_t keyLen_ = 0;
            char strChar_ = 0;
            bool escapeChar_ = false;
    };
}
