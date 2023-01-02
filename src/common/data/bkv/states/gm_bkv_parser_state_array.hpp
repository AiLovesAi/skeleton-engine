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
                size_ =  0;
                arrayTagHead_ = 0;
                arrayStart_ = 0;
                arrayTag_ = 0;
            }
            virtual void parse(BKV_Parser& parser, const char c);

        protected:
            friend class BKV_Parser;
            // Variables
            uint8_t arrayTag_ = 0;

        private:
            // Variables
            int64_t size_ = 0;
            int64_t arrayTagHead_ = 0;
            int64_t arrayStart_ = 0;
    };
}
