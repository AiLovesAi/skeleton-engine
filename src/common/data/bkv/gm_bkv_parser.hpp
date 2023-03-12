#pragma once

#include "gm_bkv.hpp"
#include "gm_bkv_buffer.hpp"
#include "states/gm_bkv_parser_state_array.hpp"
#include "states/gm_bkv_parser_state_complete.hpp"
#include "states/gm_bkv_parser_state_find_tag.hpp"
#include "states/gm_bkv_parser_state_key.hpp"
#include "states/gm_bkv_parser_state_number.hpp"
#include "states/gm_bkv_parser_state_string.hpp"

#include "../file/gm_logger.hpp"

#include <cstdint>
#include <cstring>
#include <memory>
#include <stack>

namespace game {
    class BKV_Parser {
        public:
            // Constructors
            BKV_Parser() {
                stateTree_.push(&keyState_);
            }
            
            ~BKV_Parser() {}

            // Functions
            void reset() {
                
                charactersRead_ = 0;
                stateTree_ = std::stack<BKV_Parser_State*>();
            }
            BKV_Parser_State* state() { return stateTree_.top(); }
            int64_t size() { return buffer_.head_; }
            std::shared_ptr<const uint8_t> data() {
                std::shared_ptr<const uint8_t> data(static_cast<uint8_t*>(std::malloc(buffer_.head_)), std::free);
                std::memcpy(data.get(), buffer_.bkv_, buffer_.head_);
                return data;
            }

        protected:
            friend class BKV_Parser_State;
            friend class BKV_Parser_State_Array;
            friend class BKV_Parser_State_Key;
            friend class BKV_Parser_State_Number;
            friend class BKV_Parser_State_Find_Tag;
            friend class BKV_Parser_State_String;
            friend class BKV_Parser_State_Complete;

            // Functions
            void openCompound();
            void closeCompound();
            void endKV(const char c);

            // Variables
            BKV_Parser_State_Array arrayState_;
            BKV_Parser_State_Complete completeState_;
            BKV_Parser_State_Find_Tag findTagState_;
            BKV_Parser_State_Key keyState_;
            BKV_Parser_State_Number numberState_;
            BKV_Parser_State_String stringState_;

            BKV_Buffer buffer_;
            int64_t charactersRead_ = 0;
            uint8_t tag_ = 0;
            std::stack<int32_t> depth_; // Current compound depth containing the start of the compound that flushes with head when it completes
            std::stack<BKV_Parser_State*> stateTree_; // Stack of states, the topmost of which will get called on next parse() call
    };
}
