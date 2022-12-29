#pragma once

#include "gm_bkv.hpp"
#include "states/gm_bkv_state_array.hpp"
#include "states/gm_bkv_state_complete.hpp"
#include "states/gm_bkv_state_find_tag.hpp"
#include "states/gm_bkv_state_key.hpp"
#include "states/gm_bkv_state_number.hpp"
#include "states/gm_bkv_state_string.hpp"
#include "../gm_logger.hpp"

#include <cstdint>
#include <cstring>
#include <memory>
#include <stack>
#include <vector>

namespace game {
    class BKV_Buffer {
        public:
            // Constructors
            BKV_Buffer() {
                capacity_ = BUFSIZ;
                bkv_ = static_cast<uint8_t*>(std::malloc(capacity_));
                bkv_[0] = BKV::BKV_COMPOUND;
                stateTree_.push(&keyState_);
            }
            BKV_Buffer(const int64_t capacity) : capacity_{capacity} {
                bkv_ = static_cast<uint8_t*>(std::malloc(capacity));
                bkv_[0] = BKV::BKV_COMPOUND;
                stateTree_.push(&keyState_);
            }
            
            ~BKV_Buffer() { std::free(bkv_); }

            // Functions
            void reset() {
                head_ = 0;
                tagHead_ = 0;
                valHead_ = 0;
                stateTree_ = std::stack<BKV_State*>();
            }
            BKV_State* state() { return stateTree_.top(); }
            int64_t size() { return head_; }
            std::shared_ptr<uint8_t> data() {
                std::shared_ptr<uint8_t> data(static_cast<uint8_t*>(std::malloc(head_)), std::free);
                std::memcpy(data.get(), bkv_, head_);
                return data;
            }

        protected:
            friend class BKV_State;
            friend class BKV_State_Array;
            friend class BKV_State_Key;
            friend class BKV_State_Number;
            friend class BKV_State_Find_Tag;
            friend class BKV_State_String;
            friend class BKV_State_Complete;

            // Functions
            void openCompound();
            void closeCompound();
            void endKV(const char c);

            // Variables
            BKV_State_Array arrayState_;
            BKV_State_Complete completeState_;
            BKV_State_Find_Tag findTagState_;
            BKV_State_Key keyState_;
            BKV_State_Number numberState_;
            BKV_State_String stringState_;

            uint8_t* bkv_;
            uint8_t tag_ = 0;
            int64_t capacity_     = 0; // Capacity of BKV
            int64_t head_         = 0; // Current index of BKV
            int64_t tagHead_      = 0; // Starts at current tagID and flushes with head when the key/value pair is completed
            int64_t valHead_      = 0; // Starts at current value, just after name, and flushes with head when the key/value pair is completed
            std::stack<int32_t> depth_; // Current compound depth containing the start of the compound that flushes with head when it completes
            std::stack<BKV_State*> stateTree_; // Stack of states, the topmost of which will get called on next parse() call
    };
}
