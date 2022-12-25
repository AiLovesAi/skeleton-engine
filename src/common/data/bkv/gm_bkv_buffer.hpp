#pragma once

#include "gm_bkv.hpp"
#include "states/gm_bkv_state.hpp"

#include <cstdint>
#include <cstring>
#include <memory>

namespace game {
    class BKV_Buffer {
        public:
            // Constructors
            BKV_Buffer() {
                capacity = BUFSIZ;
                bkv = static_cast<uint8_t*>(std::malloc(capacity));
                bkv[0] = BKV::BKV_COMPOUND;
            }
            BKV_Buffer(const size_t capacity) : capacity{capacity} {
                bkv = static_cast<uint8_t*>(std::malloc(capacity));
                bkv[0] = BKV::BKV_COMPOUND; // TODO Start/end with {}
            }
            
            ~BKV_Buffer() {
                std::free(bkv);
            }

            // Functions
            void reset() {
                head = 1;
                tagHead = 1;
                valHead = 1;
                state = BKV_State::nameState();
            }

            void endCompound();
            void endKV(BKV_Buffer& buf, const char c);

            // Variables
            uint8_t* bkv;
            uint8_t tag = 0;
            size_t capacity = 0; // Capacity of BKV
            size_t head     = 1; // Current index of BKV
            size_t tagHead  = 1; // Starts at current tagID and flushes with head when the keyv/value pair is completed
            size_t valHead  = 1; // Starts at current value, just after name, and flushes with head when the keyv/value pair is completed
            size_t depth    = 1; // Current compound depth
            BKV_State* state = BKV_State::nameState();
    };
}
