#pragma once

#include "gm_bkv_parser_state.hpp"

#include <memory>

namespace game {
    class BKV_Parser;
    
    class BKV_Parser_State_String : public BKV_Parser_State {
        public:
            // Constructors
            BKV_Parser_State_String() {
                _strCapacity = BUFSIZ;
                _str = static_cast<uint8_t*>(std::malloc(_strCapacity));
            }
            ~BKV_Parser_State_String() { std::free(_str); }

            // Functions
            void reset() {
                _strLen = 0;
                _strChar = 0;
                _escapeChar = false;
            }
            static char getEscapeChar(const char c);
            virtual void parse(BKV_Parser& parser, const char c);
            
        private:
            // Functions
            void continueStr(BKV_Parser& parser, const char c);
            void checkForBool(BKV_Parser& parser);
            void completeStr(BKV_Parser& parser, const char c);
            
            // Variables
            uint8_t* _str = nullptr;
            int64_t _strCapacity = 0;
            int64_t _strLen = 0;
            char _strChar = 0;
            bool _escapeChar = false;
    };
}
