#pragma once

#include "gm_bkv_parser_state.hpp"

namespace game {
    class BKV_Parser;
    
    class BKV_Parser_State_Find_Tag : public BKV_Parser_State {
        public:
            // Constructors
            BKV_Parser_State_Find_Tag() {}
            ~BKV_Parser_State_Find_Tag() {}
            
            // Functions
            virtual void parse(BKV_Parser& parser, const char c);
        
        private:
            // Functions
            void parseStr(BKV_Parser& parser, const char c);
    };
}
