#pragma once

#include "gm_bkv.hpp"

namespace game {
    class SBKV {
        public:
            // Functions
            static UTF8Str sbkvFromBKV(const BKV_t& bkv);
    };
}
