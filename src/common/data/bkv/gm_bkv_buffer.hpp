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
            }
            BKV_Buffer(const size_t capacity) : capacity{capacity} {
                bkv = static_cast<uint8_t*>(std::malloc(capacity));
            }
            
            ~BKV_Buffer() {
                std::free(bkv);
            }

            // Functions
            void reset(BKV_Buffer& buf) {
                head = 0;
                tagHead = 0;
                valHead = 0;
                state = BKV_State::nameState();
            }

            // Variables
            uint8_t* bkv;
            uint8_t tag = 0;
            size_t capacity = 0; // Capacity of BKV
            size_t head     = 0; // Current index of BKV
            size_t tagHead  = 0; // Starts at current tagID and flushes with head when the keyv/value pair is completed
            size_t valHead  = 0; // Starts at current value, just after name, and flushes with head when the keyv/value pair is completed
            BKV_State* state = BKV_State::nameState();

    };
}
