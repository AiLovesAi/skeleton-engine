#pragma once

#include "gm_bkv_parser_state.hpp"

namespace game {
    class BKV_Parser;
    
    class BKV_Parser_State_Complete : public BKV_Parser_State {
        public:
            // Constructors
            BKV_Parser_State_Complete() {}
            ~BKV_Parser_State_Complete() {}

            // Functions
            virtual void parse(BKV_Parser& parser, const char c);
    };
}