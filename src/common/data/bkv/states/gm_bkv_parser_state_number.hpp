#pragma once

#include "gm_bkv_parser_state.hpp"
#include "../../../headers/string.hpp"

namespace game {
    class BKV_Parser;
    
    class BKV_Parser_State_Number : public BKV_Parser_State { 
        public:
            // Constructors
            BKV_Parser_State_Number() {}
            ~BKV_Parser_State_Number() {}
            
            // Functions
            void reset() {
                _hasDecimal = false;
                _hasNegative = false;
                _numBuffer.clear();
            }
            virtual void parse(BKV_Parser& parser, const char c);

        private:
            // Functions
            template <typename T>
            void _appendValue(BKV_Parser& parser, const T value);
            void _endNumber(BKV_Parser& parser, const char c);

            // Variables
            StringBuffer _numBuffer;
            bool _hasDecimal = false;
            bool _hasNegative = false;
    };
}
