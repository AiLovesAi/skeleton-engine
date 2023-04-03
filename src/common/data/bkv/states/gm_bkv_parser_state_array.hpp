#pragma once

#include "gm_bkv_parser_state.hpp"

namespace game {
    class BKV_Parser;
    
    class BKV_Parser_State_Array : public BKV_Parser_State {
        public:
            // Constructors
            BKV_Parser_State_Array() {}
            ~BKV_Parser_State_Array() {}

            // Functions
            void reset() {
                _size =  0;
                _arrayTagHead = 0;
                _arrayStart = 0;
                _arrayTag = 0;
            }
            virtual void parse(BKV_Parser& parser, const char c);

        protected:
            friend class BKV_Parser;
            // Variables
            uint8_t _arrayTag = 0;

        private:
            // Variables
            int64_t _size = 0;
            int64_t _arrayTagHead = 0;
            int64_t _arrayStart = 0;
    };
}
