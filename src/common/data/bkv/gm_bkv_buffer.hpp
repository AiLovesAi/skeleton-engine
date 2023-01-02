#pragma once

#include "gm_bkv.hpp"

#include <cstdint>
#include <cstring>
#include <memory>

namespace game {
    class BKV_Buffer {
        public:
            // Constructors
            BKV_Buffer() {
                capacity_ = BUFSIZ;
                bkv_ = static_cast<uint8_t*>(std::malloc(capacity_));
                bkv_[0] = BKV::BKV_COMPOUND;
            }
            BKV_Buffer(const int64_t capacity) : capacity_{capacity} {
                bkv_ = static_cast<uint8_t*>(std::malloc(capacity));
                bkv_[0] = BKV::BKV_COMPOUND;
            }
            
            ~BKV_Buffer() { std::free(bkv_); }

            // Functions
            void reset() {
                head_ = 0;
                tagHead_ = 0;
                valHead_ = 0;
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
            uint8_t* bkv_;
            int64_t capacity_ = 0; // Capacity of BKV
            int64_t head_     = 0; // Current index of BKV
            int64_t tagHead_  = 0; // Starts at current tagID and flushes with head when the key/value pair is completed
            int64_t valHead_  = 0; // Starts at current value, just after name, and flushes with head when the key/value pair is completed
    };
}
