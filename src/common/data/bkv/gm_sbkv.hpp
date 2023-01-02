#pragma once

#include "gm_bkv.hpp"

namespace game {
    class SBKV {
        public:
            // Types
            template <typename T>
            struct BKVSuffixMap { static const char suffix[]; };
            
            // Functions
            static UTF8Str sbkvFromBKV(const BKV_t& bkv);
    };
}
