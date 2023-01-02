#pragma once

#include "gm_bkv.hpp"
#include "gm_bkv_buffer.hpp"

namespace game {
    class BKV_Builder {
        public:
            // Functions
            // TODO builder.addCompound(builder.addStr("name", "Billy").addIntArray("money", ints, 5).addCompound(builder.addInt("id", 1)).addBool("alive", true)).build();
        
        private:
            // Variables
            BKV_Buffer buffer_;
    };
}
