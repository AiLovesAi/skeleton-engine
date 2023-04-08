#pragma once

#include "gm_bkv.hpp"

#include "../../headers/string.hpp"

#include <cstdint>
#include <cstring>
#include <memory>

namespace game {
    class BKV_Buffer {
        public:
            // Constructors
            BKV_Buffer() {
                _capacity = STRBUFSIZ;
                _bkv = static_cast<uint8_t*>(std::malloc(_capacity));
                _bkv[0] = BKV::BKV_COMPOUND;
            }
            BKV_Buffer(const int64_t capacity) : _capacity{capacity} {
                _bkv = static_cast<uint8_t*>(std::malloc(capacity));
                _bkv[0] = BKV::BKV_COMPOUND;
            }
            
            ~BKV_Buffer() { std::free(_bkv); }

            // Functions
            void reset() {
                _head = 0;
                _tagHead = 0;
                _valHead = 0;
            }

        protected:
            friend class BKV_Parser;
            friend class BKV_Builder;
            friend class BKV_Parser_State;
            friend class BKV_Parser_State_Array;
            friend class BKV_Parser_State_Key;
            friend class BKV_Parser_State_Number;
            friend class BKV_Parser_State_Find_Tag;
            friend class BKV_Parser_State_String;
            friend class BKV_Parser_State_Complete;

            // Variables
            uint8_t* _bkv;
            int64_t _capacity = 0; // Capacity of BKV
            int64_t _head     = 0; // Current index of BKV
            int64_t _tagHead  = 0; // Starts at current tagID and flushes with head when the key/value pair is completed
            int64_t _valHead  = 0; // Starts at current value, just after name, and flushes with head when the key/value pair is completed
    };
}
