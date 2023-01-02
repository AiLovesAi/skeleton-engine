#pragma once

#include "gm_bkv_parser_state.hpp"

namespace game {
    class BKV_Parser;
    
    class BKV_Parser_State_Number : public BKV_Parser_State { 
        public:
            // Constructors
            BKV_Parser_State_Number() {}
            ~BKV_Parser_State_Number() {}
            
            // Functions
            void reset() {
                bufLen_ = 0;
                hasDecimal_ = false;
                hasNegative_ = false;
            }
            virtual void parse(BKV_Parser& parser, const char c);

        private:
            // Functions
            template <typename T>
            void appendValue(BKV_Parser& parser, const T value);
            template <typename T, typename TU>
            void parseInt(BKV_Parser& parser, const int64_t min, const int64_t max, const uint64_t umax);
            void parseLong(BKV_Parser& parser);
            void parseFloat(BKV_Parser& parser);
            void parseDouble(BKV_Parser& parser);
            void endNumber(BKV_Parser& parser, const char c);

            // Variables
            char numBuf_[UINT8_MAX];
            size_t bufLen_ = 0;
            bool hasDecimal_ = false;
            bool hasNegative_ = false;
    };
}
