#pragma once

#include "gm_bkv.hpp"
#include "states/gm_bkv_state_key.hpp"

#include <cstdint>
#include <cstring>
#include <memory>
#include <stack>

namespace game {
    class BKV_Buffer {
        public:
            // Constructors
            BKV_Buffer() {
                capacity_ = BUFSIZ;
                bkv_ = static_cast<uint8_t*>(std::malloc(capacity_));
                bkv_[0] = BKV::BKV_COMPOUND;
                stateTree_.push(BKV_Buffer::keyState()); // TODO Make these non-static due to dependency loops
            }
            BKV_Buffer(const int64_t capacity) : capacity_{capacity} {
                bkv_ = static_cast<uint8_t*>(std::malloc(capacity));
                bkv_[0] = BKV::BKV_COMPOUND;
                stateTree_.push(BKV_Buffer::keyState());
            }
            
            ~BKV_Buffer() {
                std::free(bkv_);
            }

            // Functions
            void reset() {
                head_ = 1;
                tagHead_ = 1;
                valHead_ = 1;
                stateTree_ = std::stack<BKV_State*>();
            }
            BKV_State* state() { return stateTree_.top(); }
            int64_t size() { return head_; }
            std::shared_ptr<uint8_t> data() { return std::shared_ptr<uint8_t>(bkv_, std::free); }

        protected:
            friend class BKV_State;
            friend class BKV_State_Key;
            friend class BKV_State_Array;
            friend class BKV_State_Number;
            friend class BKV_State_Find_Tag;
            friend class BKV_State_String;
            friend class BKV_State_Complete;

            // Functions
            void endCompound();
            void endKV(const char c);

            // Variables
            uint8_t* bkv_;
            uint8_t tag_ = 0;
            int64_t capacity_ = 0; // Capacity of BKV
            int64_t head_     = 1; // Current index of BKV
            int64_t tagHead_  = 1; // Starts at current tagID and flushes with head when the keyv/value pair is completed
            int64_t valHead_  = 1; // Starts at current value, just after name, and flushes with head when the keyv/value pair is completed
            int32_t depth_   = 1; // Current compound depth
            std::stack<BKV_State*> stateTree_; // Stack of states, the topmost of which will get called on next parse() call
    };
}
