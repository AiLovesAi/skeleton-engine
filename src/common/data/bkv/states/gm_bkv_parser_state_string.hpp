#pragma once

#include "gm_bkv_parser_state.hpp"
#include "../../../headers/string.hpp"

#include <memory>

namespace game {
    class BKV_Parser;
    
    class BKV_Parser_State_String : public BKV_Parser_State {
        public:
            // Constructors
            BKV_Parser_State_String() {}
            ~BKV_Parser_State_String() {}

            // Functions
            void reset() {
                _strChar = 0;
                _escapeChar = false;
            }
            virtual void parse(BKV_Parser& parser, const char c);
            
        private:
            // Functions
            void continueStr(BKV_Parser& parser, const char c);
            void checkForBool(BKV_Parser& parser);
            void completeStr(BKV_Parser& parser, const char c);
            
            // Variables
            StringBuffer _str;
            char _strChar = 0;
            bool _escapeChar = false;
    };
}
