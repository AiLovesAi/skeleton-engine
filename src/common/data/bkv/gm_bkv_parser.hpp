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
                _stateTree.push(&_keyState);
            }
            
            ~BKV_Parser() {}

            // Functions
            void reset() {
                _charactersRead = 0;
                _stateTree = std::stack<BKV_Parser_State*>();
            }
            BKV_Parser_State* state() { return _stateTree.top(); }
            int64_t size() { return _buffer._head; }
            std::shared_ptr<const uint8_t> data() {
                uint8_t* buffer = static_cast<uint8_t*>(std::malloc(_buffer._head));
                std::memcpy(buffer, _buffer._bkv, _buffer._head);
                std::shared_ptr<const uint8_t> data(buffer, std::free);
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
            BKV_Parser_State_Array _arrayState;
            BKV_Parser_State_Complete _completeState;
            BKV_Parser_State_Find_Tag _findTagState;
            BKV_Parser_State_Key _keyState;
            BKV_Parser_State_Number _numberState;
            BKV_Parser_State_String _stringState;

            BKV_Buffer _buffer;
            int64_t _charactersRead = 0;
            uint8_t _tag = 0;
            std::stack<int32_t> _depth; // Current compound depth containing the start of the compound that flushes with head when it completes
            std::stack<BKV_Parser_State*> _stateTree; // Stack of states, the topmost of which will get called on next parse() call
    };
}
