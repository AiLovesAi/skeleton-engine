#pragma once

#include "gm_bkv_parser_state.hpp"

#include <memory>

namespace game {
    class BKV_Parser;
    
    class BKV_Parser_State_String : public BKV_Parser_State {
        public:
            // Constructors
            BKV_Parser_State_String() {
                strCapacity_ = BUFSIZ;
                str_ = static_cast<uint8_t*>(std::malloc(strCapacity_));
            }
            ~BKV_Parser_State_String() { std::free(str_); }

            // Functions
            void reset() {
                strLen_ = 0;
                strChar_ = 0;
                escapeChar_ = false;
            }
            static char getEscapeChar(const char c);
            virtual void parse(BKV_Parser& parser, const char c);
            
        private:
            // Functions
            void continueStr(BKV_Parser& parser, const char c);
            void checkForBool(BKV_Parser& parser);
            void completeStr(BKV_Parser& parser, const char c);
            
            // Variables
            uint8_t* str_ = nullptr;
            int64_t strCapacity_ = 0;
            int64_t strLen_ = 0;
            char strChar_ = 0;
            bool escapeChar_ = false;
    };
}
