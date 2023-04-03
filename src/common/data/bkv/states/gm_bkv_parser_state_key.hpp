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
                _keyLen = 0;
                _strChar = 0;
                _escapeChar = false;
            }
            virtual void parse(BKV_Parser& parser, const char c);
            
        private:
            // Functions
            void completeKey(BKV_Parser& parser, const char c);
            void continueKey(BKV_Parser& parser, const char c);

            // Variables
            uint8_t _key[UINT8_MAX];
            int16_t _keyLen = 0;
            char _strChar = 0;
            bool _escapeChar = false;
    };
}
